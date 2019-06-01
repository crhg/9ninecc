# 開発記録

# [整数1つをコンパイルするコンパイラを作成](https://github.com/crhg/9ninecc/commit/15842e37b59a5b7ab8379e7667e86238b84b8924)

基本そのまま写経レベル。

# [足し算と引き算を追加](https://github.com/crhg/9ninecc/commit/f60357faa03e3e59b8656efed8fc5e4157b24796)

atoiからstrtolになってなるほどという感じ

# [Create README.md](https://github.com/crhg/9ninecc/commit/85a52aeac6f21e766911afbad00b2cad75a53a1)

そういえばREADMEなかったなと思って。名前の由来を書いた。

# [トークナイズするようにして空白をスキップ可能にした](https://github.com/crhg/9ninecc/commit/d702ce364ae1c9d475890e83f790ae22781f1812)

トークナイザがあるとコンパイラっぽい。

`error_at`で場所が出るのはいいね。

# [Merge branch 'master' of github.com:crhg/9ninecc](https://github.com/crhg/9ninecc/commit/62473761df76d5c9632d1edca58434095f4569c2)

前と同じような差分がまた出たのはなんでだかわからん。git難しい。

# [構文木を使ったコードジェネレータを導入(加減算のみ)](https://github.com/crhg/9ninecc/commit/96076c6f288e8d5fb66a64704be4e351fb97b848)

構文木がでてますますコンパイラらしくなった。

100個以上のトークンは来ないものとするとか割り切りが素敵。

`ND_NUM = 256`の256は何故だろうと思ったが、1文字の演算子のノードの`ty`をその文字にするためか。

`consume`の戻り値は真偽値でなく`Token *`にしてみた。

# [かけ算を追加](https://github.com/crhg/9ninecc/commit/476a9cd474b033bc8924c399fe2260463a0893cd)

演算子の優先度ごとに構文要素わけるやつ。

# [割り算を追加](https://github.com/crhg/9ninecc/commit/cd43d7abbef72ae83065edf5ce612b2dfc0703b4)

`cqo`命令ってなんだろうと思って調べたがよくみたら付録にチートシートが。

# [カッコを追加](https://github.com/crhg/9ninecc/commit/4109ccfbbccdfea512a68d42ce455e4fbeed05e1)

termって何だろう。

# [単項の'+'と'-'](https://github.com/crhg/9ninecc/commit/cd10c9c026b386c16cb96c7c6a412416aca86349)

同じ記号の演算子だと困るなーと思ったら、-xは0-xとして扱うのでノードの種類は増えないのだった。
たしかにコード生成も追加しなくていいしこれは楽。

# [比較演算子](https://github.com/crhg/9ninecc/commit/16bd1f0a33a0cfbd085e7c8ea7af2b95276e2fbb)

x86-64にはフラグからレジスタに値を設定する命令があるのか。

8bit時代はフラグで分岐するしかなかったけどパイプラインとかあると分岐したくないもんねー。

一気に演算子が増えたのでテスト書くの疲れた。

# [ファイル分割など](https://github.com/crhg/9ninecc/commit/5cd91efe6c478822494903ac453f67ba8d354a7a)

`error`や`error_at`で終わるとときどきコンパイラに怒られるので`_Noreturn`をつけた。

# [トークンの保存にベクターを使う](https://github.com/crhg/9ninecc/commit/d8bf24b6f2d8c7ed223f2e6b267304d07345a0f4)

ベクター単純なわりにつかいでがありそう。

# [1文字のローカル変数](https://github.com/crhg/9ninecc/commit/038e4cda4a87a2ec46c5b4fa7ad4d624a5d4d29b)

表題のわりにやることが盛りだくさんなステップ。

さっきベクター作ったのに`code`はまた固定配列?と思ったけど、これそのうち使わなくなりそうだしなという感じ。

コードジェネレータでエラーになるとソースのどこに対応するかわからないのはそのうち直したいと思った。

# [return文](https://github.com/crhg/9ninecc/commit/75bb0848d71b8feea1ee54c5c7ed7ede98d45869)

`is_alnum`みたいな奴標準でなかったっけと思って`isalnum`とか見つかったけど、動作がロケール依存と書いてあったのでそっ閉じ。

# [マップ](https://github.com/crhg/9ninecc/commit/88d4bfee0ffb60bdd80c9efec4bcb00bec6ad95c)

ベクターを活用して`keys`と`vals`の2本のベクター、lookupはリニアサーチという省力実装が素敵。

# [整数リテラルは整数の最小値に対応しない(いったん正の整数として解釈しているから)](https://github.com/crhg/9ninecc/commit/09d373d1b0ce3ef3d3a8728f90da6a3966402711)

どっかのslackかなにかで話題になっていたので。

# [複数文字のローカル変数](https://github.com/crhg/9ninecc/commit/6dfab008c545d6686541f4d06f2e62dc00375d41)

マップを使えば簡単。`Node`がどんどん太りそうな予感がする。

# [if文](https://github.com/crhg/9ninecc/commit/3a02af3bba1fd971d8b61c42608a2059a6af694b)

if文といいつつforやwhileのパースも書いてしまった。コード生成はifだけ。`.L`って何だろう(後でわかった)

# [while文](https://github.com/crhg/9ninecc/commit/b145d49ccc83a708ff91ab6a703a9a19687e00f9)

whileのコード生成のみ。なんかスタックが気になる。

# [while文のコード生成のバグを再現するテスト](https://github.com/crhg/9ninecc/commit/a4ce28e04a1bb866babd26c280b8bacbd0ce343d)

やはりループ毎にスタックが伸びているっぽいがあとで考えることにしてとりあえず先へ。

# [for文](https://github.com/crhg/9ninecc/commit/a2f5e64c038f8a2b77f800cfbf5b7d1fc6b85952)

for文も同じ問題がある(対処してないからねー)

文が終わったときにひとつスタックに値がある規則だとループから抜けるときと抜けないときでスタック消費をわける必要があってなんかめんどうそう。

# [式文を導入してスタック伸びる問題に対処](https://github.com/crhg/9ninecc/commit/c2014f5cd5e41c2adca8d2fc8814fafe55d912e2)

文が終わったときはスタックに値を残さない方針に変更。これで伸びなくなった。

# [ブロック](https://github.com/crhg/9ninecc/commit/611eea7faf4faee9647f44ec0089a6d1310a082a)

ベクターは便利だな。


