# Имя компилятора
CC = cl

# Флаги компиляции
CXXFLAGS = /EHsc

# Библиотеки
LIBS = ws2_32.lib

# Файлы проекта
CLIENT_SRC = client.cpp
SERVER_SRC = server.cpp

# Целевые файлы (исполняемые файлы)
CLIENT_EXE = client.exe
SERVER_EXE = server.exe

# Правило по умолчанию (сборка обоих исполняемых файлов)
all: $(CLIENT_EXE) $(SERVER_EXE)

# Правило для сборки клиентской программы
$(CLIENT_EXE): $(CLIENT_SRC)
	$(CC) $(CXXFLAGS) /Fe$(CLIENT_EXE) $(CLIENT_SRC) $(LIBS)

# Правило для сборки серверной программы
$(SERVER_EXE): $(SERVER_SRC)
	$(CC) $(CXXFLAGS) /Fe$(SERVER_EXE) $(SERVER_SRC) $(LIBS)

# Правило для очистки сгенерированных файлов
clean:
	del $(CLIENT_EXE) $(SERVER_EXE)

