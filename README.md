## 「VEIL」

3Dアクションゲーム
* 個人制作
* 開発環境：Visual Studio・C++・DirectX11
* 制作時間：約4ヶ月

## プレイ動画
https://github.com/user-attachments/assets/7d97d613-3d97-4f47-a7cf-707f75150a3c

## セールスポイント

### シェーダーによる質感表現
* 物理ベースレンダリング
* ポストプロセスエフェクト
    * ブルーム
    * スクリーンスペースリフレクション
* 特殊なシェーダー
    * セルシェーダー
    * 自動的に解像度に対応できるLCDパネルシェーダー

<p>
<img alt="feature_ibl" src="doc/images/feature_ibl.gif" height="180">
<img alt="feature_lcd" src="doc/images/feature_lcd.gif" height="180">
</p>

### 複数のカメラによる演出
自作フレームワークでレンダーテキスチャーを普通のテキスチャーと同じように設定できます
<p>
<img alt="feature_camera" src="doc/images/feature_camera.png" height="180">
</p>

### 多様な反射表現
* 平面反射
* スクリーンスペースリフレクション
* 環境マッピング

<p>
<img alt="feature_reflect" src="doc/images/feature_plane_reflect.gif" height="180">
<img alt="feature_ssr" src="doc/images/feature_ssr.png" height="180">
<img alt="feature_reflect" src="doc/images/feature_env_map.png" height="180">
</p>

### 効率的な実装
* デファードレンダリング
* GPU インスタンシング
* コンピュートシェーダー
* メモリ効率の改善