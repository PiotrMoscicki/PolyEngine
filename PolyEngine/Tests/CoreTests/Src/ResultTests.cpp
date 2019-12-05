#include <pe/Defines.hpp>
#include <catch.hpp>

#include <pe/core/utils/Result.hpp>

using namespace pe::core::utils;

namespace
{
	enum class eTestErrorType
	{
		ERROR_TYPE_1,
		ERROR_TYPE_2
	};

	Result<bool, eTestErrorType> func(int in)
	{
		if (in < 10)
			return in > 5;
		else
			return eTestErrorType::ERROR_TYPE_1;
	}
}

TEST_CASE("Result.isOk", "[Result]") 
{
	Result<int, eTestErrorType> result1 = 43;
	REQUIRE(result1.isOk() == true);

	Result<int, eTestErrorType> result2 = eTestErrorType::ERROR_TYPE_1;
	REQUIRE(result2.isOk() == false);

	Result<void, eTestErrorType> result3;
	REQUIRE(result3.isOk() == true);

	Result<void, eTestErrorType> result4 = eTestErrorType::ERROR_TYPE_1;
	REQUIRE(result4.isOk() == false);
}

TEST_CASE("Result.isErr", "[Result]")
{
	Result<int, eTestErrorType> result1 = 43;
	REQUIRE(result1.isErr() == false);

	Result<int, eTestErrorType> result2 = eTestErrorType::ERROR_TYPE_1;
	REQUIRE(result2.isErr() == true);

	Result<void, eTestErrorType> result3;
	REQUIRE(result3.isErr() == false);

	Result<void, eTestErrorType> result4 = eTestErrorType::ERROR_TYPE_1;
	REQUIRE(result4.isErr() == true);
}

TEST_CASE("Result.getValue", "[Result]")
{
	Result<int, eTestErrorType> result1 = 43;
	REQUIRE(result1.getValue() == 43);
}

TEST_CASE("Result.getError", "[Result]")
{
	Result<int, eTestErrorType> result2 = eTestErrorType::ERROR_TYPE_1;
	REQUIRE(result2.getError() == eTestErrorType::ERROR_TYPE_1);

	Result<void, eTestErrorType> result4 = eTestErrorType::ERROR_TYPE_1;
	REQUIRE(result4.getError() == eTestErrorType::ERROR_TYPE_1);
}

TEST_CASE("Result.join", "[Result]")
{
	Result<Result<int, eTestErrorType>, eTestErrorType> result1 = 43;
	REQUIRE(join(result1).getValue() == 43);

	Result<Result<int, eTestErrorType>, eTestErrorType> result2 = Result<int, eTestErrorType>(eTestErrorType::ERROR_TYPE_1);
	REQUIRE(join(result2).getError() == eTestErrorType::ERROR_TYPE_1);
	
	Result<Result<void, eTestErrorType>, eTestErrorType> result3 = Result<void, eTestErrorType>();
	REQUIRE(join(result3).isOk() == true);
	
	Result<Result<void, eTestErrorType>, eTestErrorType> result4 = eTestErrorType::ERROR_TYPE_1;
	REQUIRE(join(result4).isOk() == false);

	Result<Result<void, eTestErrorType>, eTestErrorType> result5 = eTestErrorType::ERROR_TYPE_1;
	REQUIRE(join(result4).getError() == eTestErrorType::ERROR_TYPE_1);
}

TEST_CASE("Result.bind", "[Result]")
{
	Result<int, eTestErrorType> result1 = 2;
	REQUIRE(bind(result1, func).getValue() == false);

	Result<int, eTestErrorType> result2 = 7;
	REQUIRE(bind(result2, func).getValue() == true);

	Result<int, eTestErrorType> result3 = 11;
	REQUIRE(bind(result3, func).getError() == eTestErrorType::ERROR_TYPE_1);

	Result<int, eTestErrorType> result4 = eTestErrorType::ERROR_TYPE_2;
	REQUIRE(bind(result4, func).getError() == eTestErrorType::ERROR_TYPE_2);
}

TEST_CASE("Result.and", "[Result]")
{
	{
		Result<int, eTestErrorType> result1 = 2;
		Result<bool, eTestErrorType> result2 = true;
		REQUIRE(result1.and(result2).getValue() == true);

		Result<int, eTestErrorType> result3 = eTestErrorType::ERROR_TYPE_1;
		Result<bool, eTestErrorType> result4 = true;
		REQUIRE(result3.and(result4).getError() == eTestErrorType::ERROR_TYPE_1);

		Result<bool, eTestErrorType> result5 = true;
		Result<int, eTestErrorType> result6 = eTestErrorType::ERROR_TYPE_2;
		REQUIRE(result5.and(result6).getError() == eTestErrorType::ERROR_TYPE_2);

		Result<bool, eTestErrorType> result7 = eTestErrorType::ERROR_TYPE_1;
		Result<int, eTestErrorType> result8 = eTestErrorType::ERROR_TYPE_2;
		REQUIRE(result7.and(result8).getError() == eTestErrorType::ERROR_TYPE_1);
	}
	{
		Result<void, eTestErrorType> result1;
		Result<bool, eTestErrorType> result2 = true;
		REQUIRE(result1.and(result2).getValue() == true);

		Result<void, eTestErrorType> result3 = eTestErrorType::ERROR_TYPE_1;
		Result<bool, eTestErrorType> result4 = true;
		REQUIRE(result3.and(result4).getError() == eTestErrorType::ERROR_TYPE_1);

		Result<void, eTestErrorType> result5;
		Result<int, eTestErrorType> result6 = eTestErrorType::ERROR_TYPE_2;
		REQUIRE(result5.and(result6).getError() == eTestErrorType::ERROR_TYPE_2);

		Result<void, eTestErrorType> result7 = eTestErrorType::ERROR_TYPE_1;
		Result<int, eTestErrorType> result8 = eTestErrorType::ERROR_TYPE_2;
		REQUIRE(result7.and(result8).getError() == eTestErrorType::ERROR_TYPE_1);
	}
	{
		Result<int, eTestErrorType> result1 = 2;
		Result<void, eTestErrorType> result2;
		REQUIRE(result1.and(result2).isOk() == true);

		Result<int, eTestErrorType> result3 = eTestErrorType::ERROR_TYPE_1;
		Result<void, eTestErrorType> result4;
		REQUIRE(result3.and(result4).getError() == eTestErrorType::ERROR_TYPE_1);

		Result<bool, eTestErrorType> result5 = true;
		Result<void, eTestErrorType> result6 = eTestErrorType::ERROR_TYPE_2;
		REQUIRE(result5.and(result6).getError() == eTestErrorType::ERROR_TYPE_2);

		Result<bool, eTestErrorType> result7 = eTestErrorType::ERROR_TYPE_1;
		Result<void, eTestErrorType> result8 = eTestErrorType::ERROR_TYPE_2;
		REQUIRE(result7.and(result8).getError() == eTestErrorType::ERROR_TYPE_1);
	}
}

TEST_CASE("Result.or", "[Result]")
{
	{
		Result<int, eTestErrorType> result1 = 2;
		Result<int, eTestErrorType> result2 = true;
		REQUIRE(result1.or(result2).getValue() == 2);

		Result<int, eTestErrorType> result3 = eTestErrorType::ERROR_TYPE_1;
		Result<int, eTestErrorType> result4 = 3;
		REQUIRE(result3.or(result4).getValue() == 3);
	
		Result<int, eTestErrorType> result5 = 4;
		Result<int, eTestErrorType> result6 = eTestErrorType::ERROR_TYPE_2;
		REQUIRE(result5.or(result6).getValue() == 4);
	
		Result<int, eTestErrorType> result7 = eTestErrorType::ERROR_TYPE_1;
		Result<int, eTestErrorType> result8 = eTestErrorType::ERROR_TYPE_2;
		REQUIRE(result7.or(result8).getError() == eTestErrorType::ERROR_TYPE_2);
	}
	{
		Result<void, eTestErrorType> result1;
		Result<void, eTestErrorType> result2;
		REQUIRE(result1.or(result2).isOk() == true);
	
		Result<void, eTestErrorType> result3 = eTestErrorType::ERROR_TYPE_1;
		Result<void, eTestErrorType> result4;
		REQUIRE(result3.or(result4).isOk() == true);
	
		Result<void, eTestErrorType> result5;
		Result<void, eTestErrorType> result6 = eTestErrorType::ERROR_TYPE_2;
		REQUIRE(result5.or(result6).isOk() == true);
		
		Result<void, eTestErrorType> result7 = eTestErrorType::ERROR_TYPE_1;
		Result<void, eTestErrorType> result8 = eTestErrorType::ERROR_TYPE_2;
		REQUIRE(result7.or(result8).getError() == eTestErrorType::ERROR_TYPE_2);
	}
}