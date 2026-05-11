# Expander Test Cases

## 事前準備

前提となる環境変数:
- `USER=alice`
- `HOME=/home/alice`
- `EMPTY=` (空文字)
- `A='"x"'`

## 基本ケース

Input: `echo hello`
Expected:
- `argv: [echo] [hello]`
- `redirs: (none)`

Input: `echo $USER`
Expected:
- `argv: [echo] [alice]`
- `redirs: (none)`

Input: `echo "$USER"`
Expected:
- `argv: [echo] [alice]`
- `redirs: (none)`

Input: `echo '$USER'`
Expected:
- `argv: [echo] [$USER]`
- `redirs: (none)`

Input: `echo he"$USER"'42'`
Expected:
- `argv: [echo] [healice42]`
- `redirs: (none)`

## 特殊展開

Input: `echo $?`
Expected:
- `argv[1]` が `shell.last_status` の文字列表現になる

Input: `echo $UNDEFINED`
Expected:
- `argv: [echo]`
- 未定義変数は空文字に展開される
- 未クォートで空文字になった引数ノードは削除される

Input: `echo $-`
Expected:
- `argv: [echo] [$-]`
- 変数名として不正な場合は `$` をリテラルとして残す

Input: `echo $1USER`
Expected:
- `argv: [echo] [USER]`
- `$1` は空文字として扱い、残りは通常文字として連結される

Input: `echo $EMPTY`
Expected:
- `argv: [echo] []`

## クォート除去と展開由来クォート

Input: `echo "'abc'"`
Expected:
- `argv: [echo] ['abc']`
- 外側の `"` は除去され、内側の `'` は文字として残る

Input: `echo $A`
Expected:
- `argv: [echo] ["x"]`
- 展開由来の `"` は除去しない

## redir->file 適用 (1A-11)

Input: `echo hi > "$HOME"/out.txt`
Expected:
- `argv: [echo] [hi]`
- `redirs: (> /home/alice/out.txt)`

Input: `echo hi > '$HOME'/out.txt`
Expected:
- `argv: [echo] [hi]`
- `redirs: (> $HOME/out.txt)`

Input: `cat < "$HOME"/in.txt | grep a >> "$HOME"/out.txt`
Expected:
- `cmd[0] redirs: (< /home/alice/in.txt)`
- `cmd[1] redirs: (>> /home/alice/out.txt)`

## エラー/失敗ケース

Input: メモリ確保失敗（`malloc` / `ft_strdup` / `ft_substr`）
Expected:
- `expander()` は `1` を返す
- 途中の中間バッファはリークしない
