/**
 @file CDO.hpp
 @date 15.04.21
 @copyright
	Cadabra Systems Inc
	daniil@megrabyan.pro
*/

#ifndef Chaos_Heisenbug_CDO_hpp
#define Chaos_Heisenbug_CDO_hpp

#include "../Heisenbug.hpp"
#include "../CDO/AbstractField.hpp"
#include "../CDO/Field/BigSignedInteger.hpp"
#include "../CDO/Field/IntegerField.hpp"
#include "../CDO/Field/String.hpp"
#include "../CDO/Table.hpp"
#include "../CDO/View.hpp"
#include "../CDO/Query/SelectQuery.hpp"
#include "../CDO/Providers/PostgreSQL.hpp"

namespace chaos {

	class cdo_test : public heisen_test
	{
	/** @name Constructors */
	/** @{ */
	public:
		cdo_test()
		: heisen_test("CDO")
		{
		}

		virtual ~cdo_test() = default;
	/** @} */

	/** @name Procedures  */
	/** @{ */
	protected:

		virtual void tear() override
		{
			HEISEN(TableCreation)
			HEISEN(ViewCreation)
			HEISEN(SimpleSelect)
			HEISEN(SelectFields)
            HEISEN(SelectSimpleCTE)
            HEISEN(SelectCTEWithSelect)
            HEISEN(SelectMultipleCTEs)
			HEISEN(JoinQueries)

			HEISEN(CreateClassOnlyBasic)
			HEISEN(CreateClassFromTable)
			HEISEN(CreatePrimaryKeyAndFK)
			HEISEN(CreateGeneratorBasic)
			HEISEN(CreateGeneratorWithPKandFK)

			HEISEN(DeleteConstructorFromName);
			HEISEN(DeleteConstructorFromTable);
			HEISEN(DeleteIfExistsModifier);
			HEISEN(DeleteGeneratorBasic);
			HEISEN(DeleteGeneratorEmptyTableName);
			HEISEN(DeleteGeneratorWithIfExists);

			HEISEN(InsertClassOnlySimple)
			HEISEN(InsertClassUseAllFields)
			HEISEN(InsertGeneratorSingleRow)
			HEISEN(InsertGeneratorMultipleRows)
		}
	/** @} */

	/** @name Tests */
	/** @{ */
	private:
		/**
		 * @brief Проверка создания таблицы
		 */
		void testTableCreation()
		{
			chaos::cdo::table users("users");
			IS_TRUE(users.get_fields().empty())  // empty

			// Add some data
			auto idField   = std::make_shared<chaos::cdo::signed_integer>("user_id", false, 0);
			auto nameField = std::make_shared<chaos::cdo::string>("name", true, "", 50);

			users.add_field(idField);
			users.add_field(nameField);

			IS_FALSE(users.get_fields().empty())
			ARE_EQUAL(users.get_fields().size(), 2u)

			// Check tableName
			ARE_EQUAL(users.name(), std::string("users"))

			// Check field names
			auto fields = users.get_fields();
			ARE_EQUAL(fields[0]->get_name(), std::string("user_id"))
			ARE_EQUAL(fields[1]->get_name(), std::string("name"))
		}

		/**
		 * @brief Проверка создания view
		 */
		void testViewCreation()
		{
			chaos::cdo::view myView("v_users");
			IS_TRUE(myView.get_fields().empty())

			// Add fields
			myView.add_field(std::make_shared<chaos::cdo::signed_integer>("user_id", false, 0));
			myView.add_field(std::make_shared<chaos::cdo::string>("user_name", true));

			IS_FALSE(myView.get_fields().empty())
			ARE_EQUAL(myView.get_fields().size(), 2u)

			ARE_EQUAL(myView.name(), std::string("v_users"))
		}

		/**
		 * @brief Простой SELECT-запрос c WHERE
		 */
		void testSimpleSelect()
		{
			chaos::cdo::table users("users");
			users.add_field(std::make_shared<chaos::cdo::signed_integer>("user_id", false));
			users.add_field(std::make_shared<chaos::cdo::string>("name", true));
			users.add_field(std::make_shared<chaos::cdo::signed_integer>("age", true, 18));

			// Creating the query
			chaos::cdo::select query;
			query.from(users)
				 .where(users.get_fields()[2], chaos::cdo::select::ECompareOp::Greater, 19);
				 // age > 19

			// Check for the only one WHERE
			ARE_EQUAL(query.where_conditions().size(), 1u)

			// Generate the SQL
			chaos::cdo::postgresql generator;
			auto sqlString = generator(query);

			// Check the string is not empty
			IS_FALSE(sqlString.empty())
            //LOG(sqlString.c_str())
		}

		/**
		 * @brief SELECT с выбором конкретных полей
		 */
		void testSelectFields()
		{
			chaos::cdo::table users("users");
			auto idField   = std::make_shared<chaos::cdo::signed_integer>("user_id", false);
			auto nameField = std::make_shared<chaos::cdo::string>("name", true);
			auto ageField  = std::make_shared<chaos::cdo::signed_integer>("age", true);

			users.add_field(idField);
			users.add_field(nameField);
			users.add_field(ageField);

			chaos::cdo::select query;
			// Set some fields to select
			query.fields(idField)
				 .fields(nameField)
				 .from(users);

			ARE_EQUAL(query.selectable_fields().size(), 2u)

			chaos::cdo::postgresql generator;
			auto sqlString = generator(query);
			IS_FALSE(sqlString.empty())
		}

		/**
		 * @brief Тест JOIN
		 */
		void testJoinQueries()
		{
			chaos::cdo::table users("users");
			auto uid = std::make_shared<chaos::cdo::signed_integer>("user_id", false);
			users.add_field(uid);

			chaos::cdo::table orders("orders");
			auto oid  = std::make_shared<chaos::cdo::signed_integer>("order_id", false);
			auto ouid = std::make_shared<chaos::cdo::signed_integer>("user_id", false);
			orders.add_field(oid);
			orders.add_field(ouid);

			chaos::cdo::select query;
			query.from(users)
				 .join_inner(orders)
				 .on(uid, chaos::cdo::select::ECompareOp::Equal, ouid);

			ARE_EQUAL(query.joins().size(), 1u)
			ARE_EQUAL(query.joins().at(0).on_conditions.size(), 1u)

			chaos::cdo::postgresql generator;
			auto sqlString = generator(query);

			IS_FALSE(sqlString.empty())
            //LOG(sqlString.c_str())
		}

        /**
         * @brief Тест CTE с использованием в основном запросе
         */
        void testSelectSimpleCTE()
        {
            chaos::cdo::table users("users");
            users.add_field(std::make_shared<chaos::cdo::signed_integer>("user_id", false));
            users.add_field(std::make_shared<chaos::cdo::string>("name", true));
            users.add_field(std::make_shared<chaos::cdo::signed_integer>("age", true));

            chaos::cdo::select cteQuery;
            cteQuery.fields(users.get_fields()[1]) // SELECT name
                .from(users)                  // FROM users
                .where(users.get_fields()[2], chaos::cdo::select::ECompareOp::Greater, 25); // WHERE age > 25

            chaos::cdo::select query;
            query.with(cteQuery) // Add the CTE query
                .from(users);

            // Check the number of CTEs
            ARE_EQUAL(query.with_queries().size(), 1u);

            chaos::cdo::postgresql generator;
            auto sqlString = generator(query);

            IS_FALSE(sqlString.empty());
			//LOG(sqlString.c_str());

            // Verify the generated SQL matches the expected result
            ARE_EQUAL(sqlString, "WITH cte0 AS (SELECT name FROM users WHERE age > 25) SELECT * FROM users;");
        }

        /**
         * @brief Тест CTE с использованием в основном запросе
         */
        void testSelectCTEWithSelect()
        {
            chaos::cdo::table users("users");
            users.add_field(std::make_shared<chaos::cdo::signed_integer>("user_id", false));
            users.add_field(std::make_shared<chaos::cdo::string>("name", true));
            users.add_field(std::make_shared<chaos::cdo::signed_integer>("age", true));

            chaos::cdo::select cteQuery;
            cteQuery.fields(users.get_fields()[0])
                .from(users)
                .where(users.get_fields()[2], chaos::cdo::select::ECompareOp::Greater, 30); // WHERE age > 30

            chaos::cdo::select query;
            query.with(cteQuery) // Add the CTE query
                .fields(users.get_fields()[1]) // SELECT name
                .from(cteQuery); // Use CTE as the FROM source

            // Check the number of CTEs
            ARE_EQUAL(query.with_queries().size(), 1u);
            // Ensure CTE is used as a subquery
            ARE_EQUAL(query.from_subqueries().size(), 1u);

            chaos::cdo::postgresql generator;
            auto sqlString = generator(query);

            IS_FALSE(sqlString.empty());
			//LOG(sqlString.c_str());
			//LOG("Expected:");
			//LOG("WITH cte0 AS (SELECT user_id FROM users WHERE age > 30) SELECT name FROM cte0;");


            // Verify the generated SQL matches the expected result
			ARE_EQUAL(sqlString, "WITH cte0 AS (SELECT user_id FROM users WHERE age > 30) SELECT name FROM cte0;");
        }

        /**
         * @brief Тест нескольких CTE
         */
        void testSelectMultipleCTEs()
        {
            chaos::cdo::table users("users");
            users.add_field(std::make_shared<chaos::cdo::signed_integer>("user_id", false));
            users.add_field(std::make_shared<chaos::cdo::string>("name", true));
            users.add_field(std::make_shared<chaos::cdo::signed_integer>("age", true));

            chaos::cdo::select cte1;
            cte1.fields(users.get_fields()[0]) // SELECT user_id
                .from(users)                  // FROM users
                .where(users.get_fields()[2], chaos::cdo::select::ECompareOp::Greater, 30); // WHERE age > 30

            chaos::cdo::select cte2;
            cte2.fields(users.get_fields()[1]) // SELECT name
                .from(users)                  // FROM users
                .where(users.get_fields()[2], chaos::cdo::select::ECompareOp::Less, 20); // WHERE age < 20

            chaos::cdo::select query;
            query.with(cte1) // Add the first CTE
                .with(cte2) // Add the second CTE
                .fields(users.get_fields()[1]) // SELECT name
				.from(cte1) // Use the first CTE as the FROM source
				.from(cte2); // Use the second CTE as the FROM source

            // Check the number of CTEs
            ARE_EQUAL(query.with_queries().size(), 2u);
            // Ensure CTE1 is used as a subquery
			ARE_EQUAL(query.from_subqueries().size(), 2u);

            chaos::cdo::postgresql generator;
            auto sqlString = generator(query);

            IS_FALSE(sqlString.empty());
			//LOG(sqlString.c_str());
			//LOG("Expected:");
			//LOG("WITH cte0 AS (SELECT user_id FROM users WHERE age > 30), cte1 AS (SELECT name FROM users WHERE age < 20) SELECT name FROM cte0, cte1;");

            // Verify the generated SQL matches the expected result
			ARE_EQUAL(sqlString, "WITH cte0 AS (SELECT user_id FROM users WHERE age > 30), cte1 AS (SELECT name FROM users WHERE age < 20) SELECT name FROM cte0, cte1;");
        }

		/**
		 * @brief Создание класса create c("users"), добавление колонок, проверка полей
		 */
		void testCreateClassOnlyBasic()
		{
			chaos::cdo::create c("users"); // ifNotExists=true as default
			IS_TRUE(c.use_if_not_exists());
			ARE_EQUAL(c.table_name(), std::string("users"));

			// Add single column
			c.columns({
				std::make_shared<chaos::cdo::signed_integer>("user_id", false),
				std::make_shared<chaos::cdo::string>("username", true)
			});

			ARE_EQUAL(c.columns_list().size(), 2u);
            //LOG("ClassOnlyBasic test passed");
		}

		/**
		 * @brief Создание create из объекта table
		 */
		void testCreateClassFromTable()
		{
			// Imagine, we already have a table
			chaos::cdo::table t("orders");
			t.add_field(std::make_shared<chaos::cdo::signed_integer>("order_id", false));
			t.add_field(std::make_shared<chaos::cdo::string>("desc", true));

			// Create the "create" class
			chaos::cdo::create c(t);

			ARE_EQUAL(c.table_name(), std::string("orders"));
			ARE_EQUAL(c.columns_list().size(), 2u);

            //LOG("ClassFromTable test passed");
		}

		/**
		 * @brief Проверка корректности Primary_key, Foreign_key
		 */
		void testCreatePrimaryKeyAndFK()
		{
			chaos::cdo::create c("my_table");
			c.columns({
				std::make_shared<chaos::cdo::signed_integer>("id", false),
				std::make_shared<chaos::cdo::signed_integer>("user_id", false),
				std::make_shared<chaos::cdo::string>("title", true, "", 50)
			});

			// Add pkey
			c.primary_key({"id"});

			// Add fkey
			struct chaos::cdo::create::foreign_key fk;
			fk.constraint_name = "fk_userid";
			fk.columns = {"user_id"};
			fk.ref_table = "users";
			fk.ref_columns = {"id"};
			fk.on_update = "CASCADE";
			fk.on_delete = "SET NULL";
			c.foreign_key({fk});

			// Check
			ARE_EQUAL(c.primary_keys().size(), 1u);
			ARE_EQUAL(c.primary_keys()[0], std::string("id"));

			ARE_EQUAL(c.foreign_keys().size(), 1u);
			ARE_EQUAL(c.foreign_keys()[0].constraint_name, std::string("fk_userid"));
			ARE_EQUAL(c.foreign_keys()[0].columns.size(), 1u);
			ARE_EQUAL(c.foreign_keys()[0].columns[0], std::string("user_id"));

            //LOG("PrimaryKeyAndFK test passed");
		}

		/**
		 * @brief Проверка минимального генератора
		 */
		void testCreateGeneratorBasic()
		{
			chaos::cdo::create c("users");
			c.if_not_exists(true);
			c.columns({
				std::make_shared<chaos::cdo::signed_integer>("user_id", false),
				std::make_shared<chaos::cdo::string>("username", true)
			});

			chaos::cdo::postgresql pg;
			auto sql = pg(c);
			//LOG(sql.c_str());
			// Example:
			// CREATE TABLE IF NOT EXISTS users (user_id INTEGER NOT NULL, username VARCHAR(255);
			// FYI: better go up to C++23, due to introduction of std::string::contains, which is SIGNIFICANTLY faster than find.
			IS_TRUE(sql.find("CREATE TABLE IF NOT EXISTS users (user_id INTEGER NOT NULL, username VARCHAR(255));") != std::string::npos);
            //LOG("testCreateGeneratorBasic test passed");
		}

		/**
		 * @brief Проверка генератора c PRIMARY KEY, FOREIGN KEY
		 */
		void testCreateGeneratorWithPKandFK()
		{
			chaos::cdo::create c("blog_posts", true);
			c.columns({
				std::make_shared<chaos::cdo::signed_integer>("post_id", false),
				std::make_shared<chaos::cdo::signed_integer>("author_id", false),
				std::make_shared<chaos::cdo::string>("title", true)
			});
			c.primary_key({"post_id"});
			c.foreign_key({
				{
					"fk_author_id",       // constraint_name
					{"author_id"},        // columns
					"users",              // ref_table
					{"id"},               // ref_columns
					"CASCADE",            // on_update
					"RESTRICT"            // on_delete
				}
			});

			chaos::cdo::postgresql pg;
			auto sql = pg(c);
			//LOG(sql.c_str());

			// Example:
			// CREATE TABLE IF NOT EXISTS blog_posts (post_id INTEGER NOT NULL, author_id INTEGER NOT NULL, title VARCHAR(255), PRIMARY KEY (post_id), CONSTRAINT fk_author_id FOREIGN KEY (author_id) REFERENCES users(id) ON UPDATE CASCADE ON DELETE RESTRICT);
			IS_TRUE(sql.find("CREATE TABLE IF NOT EXISTS blog_posts (post_id INTEGER NOT NULL, author_id INTEGER NOT NULL, title VARCHAR(255), PRIMARY KEY (post_id), CONSTRAINT fk_author_id FOREIGN KEY (author_id) REFERENCES users(id) ON UPDATE CASCADE ON DELETE RESTRICT);") != std::string::npos);
		}

		/**
		 * @brief Проверка создания delete класса
		 */
		void testDeleteConstructorFromName()
		{
			chaos::cdo::drop d("users");
			ARE_EQUAL(d.table_name(), "users");
			IS_FALSE(d.use_if_exists());
		}

		/**
		 * @brief Проверка создания delete класса из таблицы с ifExists == true
		 */
		void testDeleteConstructorFromTable()
		{
			chaos::cdo::table t("orders");
			chaos::cdo::drop d(t, true);
			ARE_EQUAL(d.table_name(), "orders");
			IS_TRUE(d.use_if_exists());
		}

		/**
		 * @brief Проверка изменения состояния модификатора ifExists
		 */
		void testDeleteIfExistsModifier()
		{
			chaos::cdo::drop d("products");
			IS_FALSE(d.use_if_exists());

			d.if_exists(true);
			IS_TRUE(d.use_if_exists());
		}

		/**
		 * @brief Проверка простейшего генератора
		 */
		void testDeleteGeneratorBasic()
		{
			chaos::cdo::drop d("employees");
			chaos::cdo::postgresql pg;
			std::string sql = pg(d);

			//LOG(sql.c_str());
			IS_TRUE(sql.find("DROP TABLE employees;") != std::string::npos);
		}

		/**
		 * @brief Проверка генератора с учётом выбора исключения
		 */
		void testDeleteGeneratorEmptyTableName()
		{
			try {
				chaos::cdo::drop d("");
				chaos::cdo::postgresql pg;
				pg(d);
				IS_TRUE(false); // Should not reach this point
			} catch (const std::exception& e) {
				//LOG(e.what());
				IS_TRUE(std::string(e.what()).find("table name to DELETE CANNOT be empty") != std::string::npos);
			}
		}

		/**
		 * @brief Проверка генератора с использованием if exists
		 */
		void testDeleteGeneratorWithIfExists()
		{
			chaos::cdo::drop d("sessions", true);
			chaos::cdo::postgresql pg;
			std::string sql = pg(d);

			//LOG(sql.c_str());
			IS_TRUE(sql.find("DROP TABLE IF EXISTS sessions;") != std::string::npos);
		}

		/**
		 * @brief Тест: простой insert (только класс)
		 */
		void testInsertClassOnlySimple()
		{
			chaos::cdo::insert ins("users");
			ins.columns({"id","name"})
			   .values({ 1, std::string("Bob") });

			ARE_EQUAL(ins.table_name(), std::string("users"));
			ARE_EQUAL(ins.columns_list().size(), 2u);
			ARE_EQUAL(ins.rows().size(), 1u);

			ARE_EQUAL(ins.rows()[0].size(), 2u);
		}

		/**
		 * @brief Тест: useAllFields=true
		 */
		void testInsertClassUseAllFields()
		{
			chaos::cdo::table t("items");
			t.add_field(std::make_shared<chaos::cdo::signed_integer>("item_id", false));
			t.add_field(std::make_shared<chaos::cdo::string>("title", true));


			chaos::cdo::insert ins(t, true);
			ARE_EQUAL(ins.table_name(), std::string("items"));
			ARE_EQUAL(ins.columns_list().size(), 2u);
			ARE_EQUAL(ins.columns_list()[0], std::string("item_id"));
			ARE_EQUAL(ins.columns_list()[1], std::string("title"));

			ins.values({
				std::make_shared<chaos::cdo::signed_integer>("item_id", false, 101),
				std::string("Book")
			});
			ARE_EQUAL(ins.rows().size(), 1u);
		}

		/**
		 * @brief Тест генерации с одной строкой
		 */
		void testInsertGeneratorSingleRow()
		{
			chaos::cdo::insert ins("users");
			ins.columns({"user_id","age","nickname"});

			// Values: 1) big_signed_integer, 2) int, 3) string
			ins.values({
			   std::make_shared<chaos::cdo::big_signed_integer>("some_bigint", false, 9999999999LL),
			   30,
			   std::string("NickName'sHere")
			});

			chaos::cdo::postgresql pg;
			auto sql = pg(ins);

			//LOG(sql.c_str());


			// INSERT INTO users (user_id, age, nickname) VALUES (9999999999, 30, 'NickName''sHere');
			// FYI: better go up to C++23, due to introduction of std::string::contains, which is SIGNIFICANTLY faster than find.
			IS_TRUE(sql.find("INSERT INTO users (user_id, age, nickname) VALUES (9999999999, 30, 'NickName''sHere');") != std::string::npos);
		}

		/**
		 * @brief Тест генерации с несколькими строками
		 */
		void testInsertGeneratorMultipleRows()
		{
			chaos::cdo::insert ins("test_table");
			ins.columns({"colA","colB"});

			// Add the first one
			ins.values({
				std::make_shared<chaos::cdo::signed_integer>("some_field", false, 123),
				std::string("Line1")
			});

			// add two more at once
			ins.values({
				{
					std::make_shared<chaos::cdo::big_signed_integer>("bigg", false, 1234567890123LL),
					std::string("Another'string")
				},
				{
					999, // int
					std::string("LastLine")
				}
			});

			chaos::cdo::postgresql pg;
			auto sql = pg(ins);
			//LOG(sql.c_str());

			// Expected:
			// INSERT INTO test_table (colA, colB) VALUES (123, 'Line1'), (1234567890123, 'Another''string'), (999, 'LastLine');
			IS_TRUE(sql.find("INSERT INTO test_table (colA, colB) VALUES (123, 'Line1'), (1234567890123, 'Another''string'), (999, 'LastLine');") != std::string::npos);

			// check the size equality to 3 rows
			IS_TRUE(ins.rows().size() == 3);
		}
	/** @} */
	};
}

#endif // Chaos_Heisenbug_CDO_hpp
