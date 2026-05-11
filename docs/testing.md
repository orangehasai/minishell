# Minishell テスト運用ガイド

このドキュメントは、`tests/` 配下の単体テストを追加・実行するときの
最小ルールをまとめたものです。

## 目的

- モジュールごとに独立してテストできる状態を保つ
- `Makefile` を肥大化させずにテストを追加できるようにする
- 追加した人以外でも、同じ手順で実行・保守できるようにする

## 現在の構成

```text
tests/
├── Makefile
├── include/
│   ├── test.h
│   └── env_test.h
├── common/
│   └── test_output.c
└── env/
    ├── env_test_main.c
    ├── env_test_utils.c
    ├── env_test_case_init.c
    └── env_test_case_ops.c
```

- `tests/Makefile`
  - テスト専用のビルド入口
- `tests/include/test.h`
  - 複数スイートで共有する宣言
- `tests/common/`
  - 複数スイートで使う共通実装
- `tests/<module>/`
  - モジュールごとのテスト本体

## 実行方法

リポジトリルートで実行する。

```bash
make test
make env_test
make run_env_test
```

- `make test`
  - `tests/Makefile` の `all` を呼ぶ
  - 将来的に全スイートをまとめてビルドする入口
- `make env_test`
  - `env` テストだけをビルドする
- `make run_env_test`
  - `env` テストをビルドして実行する

後片付け:

```bash
make clean
make fclean
```

## 新しいテストを追加する手順

例: `lexer` の単体テストを追加する場合

1. `tests/lexer/` を作る
2. `tests/include/lexer_test.h` を追加する
3. `lexer_test_main.c` を作る
4. ケースごとに `lexer_test_case_*.c` を分ける
5. 共通関数が必要なら `tests/common/` へ置く
6. `tests/Makefile` に `LEXER_OBJS` と
   `lexer_test` / `run_lexer_test` を追加する
7. 必要なら `all` に `lexer_test` を追加する
8. `make lexer_test` と `make -C tests run_lexer_test` で確認する

## 命名ルール

- スイートの入口: `tests/<module>/<module>_test_main.c`
- スイート内ユーティリティ:
  `tests/<module>/<module>_test_utils.c`
- ケースファイル:
  `tests/<module>/<module>_test_case_<topic>.c`
- スイート用ヘッダ:
  `tests/include/<module>_test.h`

## テスト設計ルール

- 1つのテスト関数は1つの責務だけを確認する
- 正常系だけでなく、更新系・削除系・異常系も入れる
- 文字列や構造体の所有権を意識し、コピーされているか確認する
- `readline` や REPL 全体に依存しない単体テストを優先する
- モジュール単体で難しいものだけ、後で統合テストに回す
- テストは毎回同じ結果になる入力だけを使う
- テスト内で確保したメモリは必ず解放する

## どこまで単体テストに入れるか

- 単体テストに向くもの
  - linked list 操作
  - 文字列変換
  - 構文判定
  - エラー分岐
- 統合テストに向くもの
  - `lexer -> parser -> expander` の接続
  - `executor` の `fork`, `pipe`, `dup2`
  - `readline` やシグナルを含む対話挙動

## 追加後の確認

最低限、追加したスイートについて次を確認する。

```bash
make <module>_test
make -C tests run_<module>_test
norminette tests/include/<module>_test.h tests/<module>/
```

必要なら最後にリポジトリ全体の `make` も通す。
