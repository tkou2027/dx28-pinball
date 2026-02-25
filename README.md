## 「VEIL」

3Dアクションゲーム
* 個人制作
* 開発環境：Visual Studio・C++・DirectX11
* 制作時間：約4ヶ月

## プレイ動画
https://github.com/user-attachments/assets/98f4f7b7-fe59-4c35-bf0b-da89135f1947

## セールスポイント

### シェーダーによる質感表現
* 物理ベースレンダリング
* セルシェーディング
* ポストプロセスエフェクト
    * ブルーム
    * スクリーンスペースリフレクション

<p>
<img alt="feature_ibl" src="doc/images/feature_ibl.gif" height="180">
<img alt="feature_lcd" src="doc/images/feature_lcd.gif" height="180">
<img alt="feature_reflect" src="doc/images/feature_plane_reflect.gif" height="180">
</p>

### 複数のカメラによる演出
自作フレームワークでレンダーテキスチャーを普通のテキスチャーと同じように設定できます
<p>
<img alt="feature_camera" src="doc/images/feature_camera.png" height="180">
</p>


### 効率的な実装
* デファードレンダリング
* GPU インスタンシング
* コンピュートシェーダー
* メモリ効率の改善