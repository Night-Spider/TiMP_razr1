//communicator.cpp
#include "communicator.h"
#include "error.h"
#include <iostream>
#include <vector>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <random>
#include <sstream>
#include <iomanip>
#include <cryptlib.h>
#include <sha.h>
#include <hex.h>
#include <string>

std::string communicator::sha256(const std::string& input_str) {
    using namespace CryptoPP;

    byte digest[SHA256::DIGESTSIZE];
    SHA256 hash;

    hash.CalculateDigest(digest, (const byte*)input_str.c_str(), input_str.size());

    // Преобразуем в шестнадцатеричное представление
    std::string hexString;
    HexEncoder encoder;
    encoder.Attach(new StringSink(hexString));
    encoder.Put(digest, sizeof(digest));
    encoder.MessageEnd();

    return hexString;
}

uint64_t communicator::generate_salt() {
    std::random_device rd;
    std::uniform_int_distribution<uint64_t> dist(0, 0xFFFFFFFFFFFFFFFF); // Генерируем 64-битное число
    return dist(rd); // Возвращаем соль как 64-битное число
}

std::string communicator::salt_to_hex(uint64_t salt) {
    std::stringstream ss;
    ss << std::hex << std::setw(16) << std::setfill('0') << salt; // Преобразуем в шестнадцатеричное представление
    return ss.str(); // Возвращаем строку в шестнадцатеричном формате
}

int communicator::connection(uint32_t port, const std::map<std::string, std::string>& base, logger* l) {
    try {
        int queue_len = 100;
        sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        inet_aton("127.0.0.1", &addr.sin_addr);
        int s = socket(AF_INET, SOCK_STREAM, 0);
        if (s <= 0) {
            throw crit_err("Ошибка создания сокета.");
        } else {
            l->writelog("Сокет создан");
        }

        if (bind(s, (const sockaddr*)&addr, sizeof(sockaddr_in)) == -1) {
            throw crit_err("Ошибка привязки сокета.");
        } else {
            l->writelog("Привязка сокета успешна");
        }

        if (listen(s, queue_len) == -1) {
            throw crit_err("Ошибка прослушивания сокета.");
        }

        for (;;) {
            sockaddr_in client_addr;
            socklen_t len = sizeof(sockaddr_in);
            int work_sock = accept(s, (sockaddr*)(&client_addr), &len);
            if (work_sock <= 0) {
                throw no_crit_err("Ошибка сокета.");
            }
            l->writelog("Сокет создан для клиента");

            // 2. Получаем идентификатор LOGIN
            char login[256];
            int rc = recv(work_sock, login, sizeof(login), 0);
            if (rc <= 0) {
                close(work_sock);
                throw no_crit_err("Ошибка получения идентификатора LOGIN");
            }
            login[rc] = '\0'; // Завершаем строку

            // 3. Проверяем идентификатор и отправляем SALT
            auto it = base.find(login);
            if (it == base.end()) {
                send(work_sock, "ERR", 3, 0);
                close(work_sock);
                continue; // Завершаем соединение
            }

            // Генерируем соль
            uint64_t salt = generate_salt();

            // Преобразуем соль в шестнадцатеричное представление
            std::string hex_salt = salt_to_hex(salt);

            // Отправляем соль (16 символов для 64-битного числа в шестнадцатеричном формате)
            if (send(work_sock, hex_salt.c_str(), hex_salt.size(), 0) == -1) {
                close(work_sock);
                throw no_crit_err("Ошибка отправки SALT");
            }

            // 4. Получаем HASH
            char hash[64];
            rc = recv(work_sock, hash, sizeof(hash), 0);
            if (rc <= 0) {
                close(work_sock);
                throw no_crit_err("Ошибка получения HASH");
            }
            hash[rc] = '\0'; // Завершаем строку

            // Проверяем HASH
            std::string expected_hash = sha256(hex_salt + it->second);
            if (expected_hash != hash) {
                // Если хэш не совпадает, отправляем ошибку
                send(work_sock, "ERR", 3, 0);
                close(work_sock);
                continue;
            }

            send(work_sock, "OK", 2, 0); // Успешная аутентификация

            // 6. Начинаем обмен в двоичном формате
            int num_vectors;
            // Получаем количество векторов
            recv(work_sock, &num_vectors, sizeof(num_vectors), 0);

            for (int i = 0; i < num_vectors; i++) {
                int vector_size;
                // Получаем количество элементов вектора
                recv(work_sock, &vector_size, sizeof(vector_size), 0);
                
                std::vector<uint32_t> vector(vector_size);

                // Получаем вектор из клиента
                size_t total_bytes_received = 0;
                while (total_bytes_received < vector_size * sizeof(uint32_t)) {
                    int bytes_received = recv(work_sock, (char*)vector.data() + total_bytes_received, vector_size * sizeof(uint32_t) - total_bytes_received, 0);
                    if (bytes_received <= 0) {
                        close(work_sock);
                        throw no_crit_err("Ошибка получения вектора, получено меньше данных");
                    }
                    total_bytes_received += bytes_received;
                }

                // Логируем вектор
                std::ostringstream vector_stream;
                vector_stream << "Вектор " << i + 1 << ": ";
                for (const auto& value : vector) {
                    vector_stream << value << " "; // Добавляем значения вектора в строку
                }
                l->writelog(vector_stream.str()); // Записываем строку в лог

                // Обрабатываем вектор и вычисляем произведение
                uint32_t result = 1;
                bool contains_zero = false;
                for (const auto& value : vector) {
                    if (value == 0) {
                        contains_zero = true;
                    }
                    if (result > std::numeric_limits<uint32_t>::max() / value) {
                        result = 4294967295; // Устанавливаем результат в 4294967295 при переполнении
                        break; // Прерываем цикл, так как переполнение уже произошло
                    }
                    result *= value; // Вычисляем произведение
                }

                if (contains_zero) {
                    l->writelog("Вектор " + std::to_string(i + 1) + " содержит 0");
                }

                // Отправляем результат обратно клиенту
                send(work_sock, &result, sizeof(result), 0);
                l->writelog("Результат для вектора " + std::to_string(i + 1) + ": " + std::to_string(result));
            }

            close(work_sock); // Закрываем сокет для клиента после обработки
        }
    } catch (crit_err& e) {
        l->writelog(e.what());
    }
    return 0;
}
