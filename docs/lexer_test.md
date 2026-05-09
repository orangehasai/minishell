# Lexer Test Cases

## 基本ケース

Input: `echo hello`
Expected:
- `WORD: echo`
- `WORD: hello`
- `EOF: NULL`

Input: `ls -la`
Expected:
- `WORD: ls`
- `WORD: -la`
- `EOF: NULL`

## クォート

Input: `echo "hello world"`
Expected:
- `WORD: echo`
- `WORD: "hello world"`
- `EOF: NULL`

Input: `echo 'hello world'`
Expected:
- `WORD: echo`
- `WORD: 'hello world'`
- `EOF: NULL`

Input: `echo he"ll"o`
Expected:
- `WORD: echo`
- `WORD: he"ll"o`
- `EOF: NULL`

Input: `echo "a | b"`
Expected:
- `WORD: echo`
- `WORD: "a | b"`
- `EOF: NULL`

## パイプ

Input: `ls | cat`
Expected:
- `WORD: ls`
- `PIPE: |`
- `WORD: cat`
- `EOF: NULL`

Input: `ls | grep minishell | wc -l`
Expected:
- `WORD: ls`
- `PIPE: |`
- `WORD: grep`
- `WORD: minishell`
- `PIPE: |`
- `WORD: wc`
- `WORD: -l`
- `EOF: NULL`

## ダイレクト

Input: `cat < in > out`
Expected:
- `WORD: cat`
- `REDIR_IN: <`
- `WORD: in`
- `REDIR_OUT: >`
- `WORD: out`
- `EOF: NULL`

Input: `cat << EOF`
Expected:
- `WORD: cat`
- `HEREDOC: <<`
- `WORD: EOF`
- `EOF: NULL`

Input: `echo hello >> out`
Expected:
- `WORD: echo`
- `WORD: hello`
- `APPEND: >>`
- `WORD: out`
- `EOF: NULL`

## 複合ケース

Input: `echo "hello $USER" | cat -e > out.txt`
Expected:
- `WORD: echo`
- `WORD: "hello $USER"`
- `PIPE: |`
- `WORD: cat`
- `WORD: -e`
- `REDIR_OUT: >`
- `WORD: out.txt`
- `EOF: NULL`

## エラーケース

Input: `echo "hello`
Expected:
- `lexer()` returns `NULL`

Input: `echo 'hello`
Expected:
- `lexer()` returns `NULL`
