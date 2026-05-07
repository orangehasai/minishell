# Parser Test Cases

## 基本ケース

Input: `echo hello`
Expected:
- `cmd[0]`
- `argv: [echo] [hello]`
- `redirs: (none)`

Input: `ls -la`
Expected:
- `cmd[0]`
- `argv: [ls] [-la]`
- `redirs: (none)`

## パイプ

Input: `ls | cat`
Expected:
- `cmd[0]`
- `argv: [ls]`
- `redirs: (none)`
- `cmd[1]`
- `argv: [cat]`
- `redirs: (none)`

Input: `echo hello | grep h | wc -l`
Expected:
- `cmd[0]`
- `argv: [echo] [hello]`
- `redirs: (none)`
- `cmd[1]`
- `argv: [grep] [h]`
- `redirs: (none)`
- `cmd[2]`
- `argv: [wc] [-l]`
- `redirs: (none)`

## リダイレクト

Input: `cat < infile > outfile`
Expected:
- `cmd[0]`
- `argv: [cat]`
- `redirs: (< infile) (> outfile)`

Input: `cat << EOF`
Expected:
- `cmd[0]`
- `argv: [cat]`
- `redirs: (<< EOF)`

Input: `echo hello >> out.txt`
Expected:
- `cmd[0]`
- `argv: [echo] [hello]`
- `redirs: (>> out.txt)`

## 複合ケース

Input: `cat < in.txt | grep abc >> out.txt`
Expected:
- `cmd[0]`
- `argv: [cat]`
- `redirs: (< in.txt)`
- `cmd[1]`
- `argv: [grep] [abc]`
- `redirs: (>> out.txt)`

Input: `> out.txt echo hi`
Expected:
- `cmd[0]`
- `argv: [echo] [hi]`
- `redirs: (> out.txt)`

## エラーケース

Input: `| ls`
Expected:
- `syntax error near unexpected token \`|'`
- `$? = 2`

Input: `echo hi | | cat`
Expected:
- `syntax error near unexpected token \`|'`
- `$? = 2`

<!-- FIXME:このケースは標準入力を受け付ける形式にしなくてはいけないが05/07時点ではエラー扱いに
          しているため修正が必要
    TODO:2-x系でシグナル系が完了後修正に着手する
-->
Input: `echo hi |`
Expected:
- `>` (継続入力プロンプト)
- その時点では構文エラーを出さない

Input: `echo >`
Expected:
- `syntax error near unexpected token \`newline'`
- `$? = 2`

Input: `cat <<`
Expected:
- `syntax error near unexpected token \`newline'`
- `$? = 2`
