#include <Jass.h>
#include <Jass/Core/EntryPoint.h>

//#include "DX11TestLayer.h"
//#include "Examples/ModelLoading/ModelLoading.h"
//#include "Examples/Transformations/Transformations.h"
//#include "Examples/TerrainLoading/TerrainLoading.h"
//#include "Examples/SkyboxLoading/SkyboxLoading.h"
//#include "Examples/BillboardLoading/BillboardLoading.h"
//#include "Examples/WaterLoading/WaterLoading.h"
//#include "Examples/SpecularMaps/SpecularMaps.h"
#include "Scene/Island.h"
//#include "Sandbox3D/Sandbox3D.h"

/*
	This is the main class of your application. It is divided into layers.
	Your program probably will only need one layer, but you can experiment with applications
	with more than one layer.

	The following code can load some examples. Please check each in the order they are.
	To load an example, uncomment its corresponding #include and its corresponding PushLayer.
	ONLY PUSH ONE LAYER AT A TIME FOR EXAMPLES! SO IF YOU UNCOMMENT ONE, COMMENT THE LAST ONE ACTIVE.

	Check the examples to know how to implement you own layer. Also, the examples give you
	information about some features already include with the program (Terrain, models, water...)

	All the examples have informative comments, but there are some things in some examples that is
	explained in other ones, so please read and try them in order.

	Thank you, enjoy and good luck!
	- Bryan Duarte
*/
class Sandbox : public Jass::Application {

public:
	Sandbox() {
		//PushLayer(new DX11TestLayer);
		//PushLayer(new ModelLoading);
		//PushLayer(new Transformations);
		//PushLayer(new TerrainLoading);
		//PushLayer(new SkyboxLoading);
		//PushLayer(new BillboardLoading);
		PushLayer(new Island);
		//PushLayer(new SpecularMaps);
		//PushLayer(new Sandbox3D);
	}

	~Sandbox() {

	}

};

Jass::Application* Jass::CreateApplication() {
	return new Sandbox();
}