# vgFuzz

実装機能:
専用入力(grammar-mmutator)
    必要な作業
    - afl++にgrammar-mutatorをインストール(公式docを参照)
        - 環境変数にcustom_mutator_onlyを設定
    - vgFuzz kitで対象プログラムに対応したgrammrファイル(json)を作成(要マニュアル,wip)
    - 規定のコマンドを入力して開始

人力インストゥルメーション(ijon)
    必要な作業
    - vgfuzzでijon_modeを1に設定
    - 対象のプログラムのソースコードに適切なIJON_SETを記述
    - 規定のコマンドを入力して開始