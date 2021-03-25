#include "jasspch.h"
#include "DCBTesting.h"

void TestDCB()
{
	{
		Dcb::RawLayout lay;
		lay.Add(Dcb::Type::Struct, "base");
		lay["base"].Add(Dcb::Type::Float3, "pos");
		lay["base"].Add(Dcb::Type::Float, "multiple");
		lay.Add(Dcb::Type::Float, "factor");
		lay.Add(Dcb::Type::Array, "myArray");
		lay["myArray"].Set(Dcb::Type::Struct, 4);
		lay["myArray"].GetArray().Add(Dcb::Type::Float3, "normal");
		lay["myArray"].GetArray().Add(Dcb::Type::Array, "intArray");
		lay["myArray"].GetArray()["intArray"].Set(Dcb::Type::Float, 2);
		lay["myArray"].GetArray().Add(Dcb::Type::Array, "sndArray");
		lay["myArray"].GetArray()["sndArray"].Set(Dcb::Type::Array, 2);
		lay["myArray"].GetArray()["sndArray"].GetArray().Set(Dcb::Type::Matrix4, 4);
		lay["myArray"].GetArray().Add(Dcb::Type::Boolean, "booler");

		//lay.Add(Dcb::Type::Boolean, "myArray"); // Correctly throws an error (duplicate)
		//lay.Add(Dcb::Type::Boolean, "1myArray"); // Correctly throws an error (invalid symbol name)

		auto buf = Dcb::Buffer(std::move(lay));

		const auto sig = buf.GetRootLayoutElement().GetSignature();

		//buf["factor"] = "Hello"; // Correctly fails to compile (non-compatible type)

		{
			auto example = 42.0f;
			buf["factor"] = example;
			float recover = buf["factor"];
			assert(recover == example);
		}
		{
			auto example = 42.0f;
			buf["base"]["multiple"] = example;
			float recover = buf["base"]["multiple"];
			assert(example == recover);
		}
		{
			auto example = 42.0f;
			buf["myArray"][2]["intArray"][0] = example;
			float recover = buf["myArray"][2]["intArray"][0];
			assert(example == recover);
		}
		{
			auto example = Jass::JVec3(1.0f, 1.0f, 1.0f);
			buf["base"]["pos"] = example;
			Jass::JVec3 recover = buf["base"]["pos"];
			assert(example == recover);
		}
		{
			Jass::JMat4 example = Jass::JMat4(1.0f);
			buf["myArray"][2]["sndArray"][0][2] = example;
			Jass::JMat4 recover = buf["myArray"][2]["sndArray"][0][2];
			assert(example == recover);
		}
		{
			auto example = true;
			buf["myArray"][1]["booler"] = example;
			bool recover = buf["myArray"][1]["booler"];
			assert(example == recover);
		}
		{
			assert(buf["base"]["pos"].Exists());
			assert(!buf["base"]["position"].Exists());

			auto ref = buf["base"]["pos"];
			if (ref.Exists())
			{
				Jass::JVec3 position = ref;
				assert(position.x == 1.0f);
			}
		}
		{
			assert(buf["base"]["pos"].SetIfExists(Jass::JVec3(2.5f, 4.0f, 2.0f)));
			auto& f3 = static_cast<const Jass::JVec3&>(buf["base"]["pos"]);
			assert(f3 == Jass::JVec3(2.5f, 4.0f, 2.0f));
			assert(!buf["base"]["position"].SetIfExists(Jass::JVec3(1.0f, 2.0f, 3.0f)));
		}
		{
			const auto& cBuf = buf;
			Jass::JMat4 recover = cBuf["myArray"][2]["sndArray"][0][2];
			assert(recover[1][1] == 1.0f);

			// Correctly does not compile because it is const
			/*cBuf["myArray"][1]["booler"] = true;
			static_cast<bool&>(cBuf["myArray"][1]["booler"]) = true;*/

			//cBuf["myArray"][200]; // Correctly fails because the index is out of bounds
		}
	}
	{
		Dcb::RawLayout lay;
		lay.Add(Dcb::Type::Array, "array");
		lay["array"].Set(Dcb::Type::Array, 6);
		lay["array"].GetArray().Set(Dcb::Type::Matrix4, 4);

		auto buf = Dcb::Buffer(std::move(lay));

		auto size = buf.GetSizeInBytes();
		assert(size == 16u * 4u * 4u * 6u);
	}
	{
		Dcb::RawLayout lay;
		lay.Add(Dcb::Type::Array, "array");
		lay["array"].Set(Dcb::Type::Struct, 6);
		lay["array"].GetArray().Add(Dcb::Type::Float2, "a");
		lay["array"].GetArray().Add(Dcb::Type::Float3, "b");

		auto buf = Dcb::Buffer(std::move(lay));

		auto size = buf.GetSizeInBytes();
		assert(size == 16u * 2u * 6u);
	}
	{
		Dcb::RawLayout lay;
		lay.Add(Dcb::Type::Array, "array");
		lay["array"].Set(Dcb::Type::Float3, 6);
		
		auto buf = Dcb::Buffer(std::move(lay));

		auto size = buf.GetSizeInBytes();
		assert(size == 16u * 6u);
	}
	{
		Dcb::RawLayout lay;
		lay.Add(Dcb::Type::Array, "array");
		lay["array"].Set(Dcb::Type::Float3, 6);

		auto fixed = Dcb::LayoutCodex::Resolve(std::move(lay));

		lay.Add(Dcb::Type::Float, "array"); // rawlayout is now cleared

		//fixed["array"].Add(Dcb::Type::Float, "test"); // Correctly does not compile because is immutable

		auto buf = Dcb::Buffer(fixed);
		buf["array"][0] = Jass::JVec3(3.0f, 3.0f, 3.0f);

		auto buf2 = Dcb::Buffer(fixed);
		buf2["array"][0] = Jass::JVec3(4.0f, 4.0f, 4.0f);

		assert(static_cast<Jass::JVec3>(buf["array"][0]).x == 3.0f);
		assert(static_cast<Jass::JVec3>(buf2["array"][0]).x == 4.0f);
	}
	{
		Dcb::RawLayout lay;
		lay.Add(Dcb::Type::Float3, "materialColor");
		lay.Add(Dcb::Type::Float3, "specularColor");
		lay.Add(Dcb::Type::Float, "specularWeight");
		lay.Add(Dcb::Type::Float, "specularGloss");

		auto fixed = Dcb::LayoutCodex::Resolve(std::move(lay));

		assert(fixed.GetSizeInBytes() == 48u);
	}
	{
		Dcb::RawLayout lay;
		lay.Add(Dcb::Type::Array, "myArray");
		lay["myArray"].Set(Dcb::Type::Float, 10);

		auto fixed = Dcb::LayoutCodex::Resolve(std::move(lay));

		assert(fixed.GetSizeInBytes() == 160u);
	}
	{
		Dcb::RawLayout lay;
		lay.Add(Dcb::Type::Array, "myArray");
		lay["myArray"].Set(Dcb::Type::Struct, 10);
		lay["myArray"].GetArray().Add(Dcb::Type::Float3, "x");
		lay["myArray"].GetArray().Add(Dcb::Type::Float2, "y");

		auto fixed = Dcb::LayoutCodex::Resolve(std::move(lay));

		assert(fixed.GetSizeInBytes() == 320u);
	}
}