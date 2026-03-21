---
description: "valgrind でメモリリークを検出し、修正箇所を特定する"
---

## 手順

1. ビルド確認:

```bash
make re
```

2. valgrind 実行（テストコマンドを流し込む）:

```bash
echo 'echo hello
ls | grep .c
export FOO=bar
env
unset FOO
exit' | valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./minishell 2>&1
```

3. 出力を分析し、**自前コードのリークのみ** を報告してください。
   readline 由来のリーク（`readline`、`add_history` 経由のもの）は無視して構いません。

4. リークが見つかった場合、該当するソースコードの行を特定し、修正案を提示してください。

## よくあるリークパターン

- `ft_strjoin` / `ft_strdup` / `ft_substr` の中間値の free 漏れ
- `ft_split` の戻り値の free 漏れ（配列と各要素の両方を free する必要あり）
- `env_to_array` の戻り値の free 漏れ
- `getcwd(NULL, 0)` の戻り値の free 漏れ
- エラーパス（early return）での確保済みメモリの free 漏れ
- `readline` の戻り値の free 漏れ（ループの各イテレーション末尾で free）
