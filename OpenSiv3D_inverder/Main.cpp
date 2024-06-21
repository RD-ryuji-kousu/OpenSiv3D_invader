# include <Siv3D.hpp> // Siv3D v0.6.14
class Player {
private:
	int life;
	Texture jiki{ U"inverder_png/jiki.png" };
	Vec2 plpos;
public:
	Player(int _life, Vec2 _plpos, Size txsz):life(_life), plpos(_plpos){
		jiki(plpos, txsz);
	}
	void move(){

	}
	void Draw() {

	}

};
class Shot {
private:
	Size sz;
	bool bullet_max;
	Texture beam{ U"inverder_png/beam1.png" };
	Vec2 bpos;
	ColorF beamc;
public:
	Shot(Size _sz, Vec2 _bpos):bullet_max(false), sz(_sz), bpos(_bpos) {
		beam(bpos, sz);
		beamc = Palette::Seagreen;
	}
	void Draw() {
		beam.draw(beamc);
	}
	bool calc_shot() {

	}
};

class Enemy {
private:
	Size txsz;
	Texture enemy;
	ColorF color;
	Vec2 epos;
public:
	Enemy(Texture enemy_pos, ColorF _color, Size _txsz, Vec2 _epos) :
		enemy(enemy_pos),color(_color), txsz(_txsz), epos(_epos){
	}
};

class Enemies {
private:
	Array<Enemy> enemies;
public:
	Enemies(Size txsz) {
		ColorF enmyc = { 0, 0, 0 };
		FilePathView path = U".";
		for (int y = 0; y < 5; y++) {
			for (int x = 100; x < (Scene::Width() - 100) / txsz.x; x++) {
				switch (y)
				{
				
				case 0:
					enmyc = { Palette::Magenta };
					path = U"inverder_png/enemy1.png";
				case 1:
					enmyc = { Palette::Magenta };
					path = U"inverder_png/enemy1.png";
				case 2:
					enmyc = { Palette::Aqua };
					path = U"inverder_png/enemy2.png";
				case 3:
					enmyc = { Palette::Aqua };
					path = U"inverder_png/enemy2.png";

				case 4:
					enmyc = { Palette::Greenyellow };
					path = U"inverder_png/enemy3.png";

				}
				enemies << Enemy(Texture(path), enmyc, txsz, Vec2(x * txsz.x, 400 - y * txsz.y));
			}
		}
	}
};

class Wall {
private:
	Size sz;
	Texture wall{ U"inverder_png/zangou.png" };
	ColorF color;
	Vec2 wpos;
public:
	Wall(Size _sz, Vec2 _wpos) :sz(_sz), wpos(_wpos) {
		color = Palette::Red;
	}
};

class Walls {
private:
	Array<Wall> walls;
public:
	Walls(Size sz) {
		for (int i = 0; i < 4; i++) {
			walls << Wall(sz, Vec2(150 + (i * sz.x), 500));
		}
	}
};

void Main()
{
	// 背景の色を設定する | Set the background color
	Scene::SetBackground(ColorF{ Palette::Black });

	Size txsz_chara{ 30, 30 };
	Size txsz_bullet{ 10, 10 };
	Size txsz_obj{ 100,100 };

	Rect debug_rect{ Arg::center(400, 300), 30, 30 };
	


	while (System::Update())
	{
		debug_rect.draw();
	}
}

//
// - Debug ビルド: プログラムの最適化を減らす代わりに、エラーやクラッシュ時に詳細な情報を得られます。
//
// - Release ビルド: 最大限の最適化でビルドします。
//
// - [デバッグ] メニュー → [デバッグの開始] でプログラムを実行すると、[出力] ウィンドウに詳細なログが表示され、エラーの原因を探せます。
//
// - Visual Studio を更新した直後は、プログラムのリビルド（[ビルド]メニュー → [ソリューションのリビルド]）が必要な場合があります。
//
