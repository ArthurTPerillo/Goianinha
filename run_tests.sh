#!/bin/bash

TEST_DIR1="testes_sintatico"
TEST_DIR2="testes_semantico"

EXEC="./goianinha"

mkdir -p outputs_sintatico
mkdir -p outputs_semantico

for file in "$TEST_DIR1"/*.g; do
    
    base=$(basename "$file" .g)
    echo "Rodando teste: $file"
    "$EXEC" "$file" > "outputs_sintatico/$base.out" 2>&1
done

for file in "$TEST_DIR2"/*.g; do
    
    base=$(basename "$file" .g)
    echo "Rodando teste: $file"
    "$EXEC" "$file" > "outputs_semantico/$base.out" 2>&1
done

echo "Testes concluídos! Veja os arquivos em outputs_sintatico e outputs_semantico/"
