namespace aho {
	class Aho {
		int a;
	};
};


namespace hoge {
	class Moge : public aho::Aho {
		
	};
};

int main(int argc, char **argv )
{
	hoge::Moge *m = new hoge::Moge();
	
}