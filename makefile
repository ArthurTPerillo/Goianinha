# Makefile para o Compilador Goianinha


# Compilador e flags
CC = gcc
CFLAGS = -Wall -Wno-unused-function -g
LEX = flex
YACC = bison

# Arquivos fonte
LEXER = goianinha.l
PARSER = goianinha.y

# Nome do executável final
TARGET = goianinha

# Arquivos gerados pelo Flex e Bison
LEX_OUTPUT = lex.yy.c
YACC_OUTPUT = goianinha.tab.c
YACC_HEADER = goianinha.tab.h

# Arquivos objeto (.o) necessários
OBJS = goianinha.tab.o lex.yy.o ast.o print_ast.o table.o semantic.o main.o


all: $(TARGET)
	@echo ""
	@echo "Compilação concluída com sucesso!"
	@echo ""


$(TARGET): $(OBJS)
	@echo "Linkando objetos para gerar executável..."
	$(CC) $(CFLAGS) -o $@ $(OBJS) -lfl
	@echo "Executável '$(TARGET)' criado"


$(YACC_OUTPUT) $(YACC_HEADER): $(PARSER)
	@echo "Gerando parser com Bison..."
	$(YACC) -d $(PARSER)
	@echo "Arquivos $(YACC_OUTPUT) e $(YACC_HEADER) gerados"


$(LEX_OUTPUT): $(LEXER) $(YACC_HEADER)
	@echo "Gerando lexer com Flex..."
	$(LEX) $(LEXER)
	@echo "Arquivo $(LEX_OUTPUT) gerado"


goianinha.tab.o: $(YACC_OUTPUT)
	@echo "Compilando parser..."
	$(CC) $(CFLAGS) -c $(YACC_OUTPUT) -o goianinha.tab.o


lex.yy.o: $(LEX_OUTPUT)
	@echo "Compilando lexer..."
	$(CC) $(CFLAGS) -c $(LEX_OUTPUT) -o lex.yy.o


ast.o: ast.c ast.h types.h
	@echo "Compilando ast.c..."
	$(CC) $(CFLAGS) -c ast.c -o ast.o


print_ast.o: print_ast.c print_ast.h ast.h types.h
	@echo "Compilando print_ast.c..."
	$(CC) $(CFLAGS) -c print_ast.c -o print_ast.o


main.o: main.c ast.h print_ast.h
	@echo "Compilando main.c..."
	$(CC) $(CFLAGS) -c main.c -o main.o


clean:
	@echo "Limpando arquivos gerados..."
	rm -f $(TARGET) $(OBJS) $(LEX_OUTPUT) $(YACC_OUTPUT) $(YACC_HEADER)
	@echo "Limpeza concluída"


distclean: clean
	@echo "Limpando backups e temporários..."
	rm -f *~ *.bak
	@echo "Limpeza completa concluída"


test: $(TARGET)
	@echo ""
	@echo "=========================================="
	@echo "        TESTANDO COMPILADOR"
	@echo "=========================================="
	@echo ""
	./$(TARGET) teste.goi


rebuild: clean all


info:
	@echo "Compilador: $(CC)"
	@echo "Flags: $(CFLAGS)"
	@echo "Lexer: $(LEX)"
	@echo "Parser: $(YACC)"
	@echo "Executável: $(TARGET)"
	@echo "Objetos: $(OBJS)"


.PHONY: all clean distclean test rebuild info
