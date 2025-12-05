#!/bin/bash

TEST_DIR="testes"

EXEC="./goianinha"

mkdir -p outputs

for file in "$TEST_DIR"/*.g; do
    
    base=$(basename "$file" .g)
    echo "Rodando teste: $file"
    "$EXEC" "$file" > "outputs/$base.out" 2>&1
done

echo "✔ Testes concluídos! Veja os arquivos em outputs/"
