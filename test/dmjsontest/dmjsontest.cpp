#include "dmjson.h"
#include "gtest.h"
#include "dmformat.h"
#include <fstream>
#include <iostream>
#include <string>

using json = nlohmann::json;

class env_dmjson {
public:
    void init() {}
    void uninit() {}
};

class frame_dmjson : public testing::Test {
public:
    virtual void SetUp() {
        fmt::print("nlohmann/json version: {}.{}.{}\n", NLOHMANN_JSON_VERSION_MAJOR, NLOHMANN_JSON_VERSION_MINOR, NLOHMANN_JSON_VERSION_PATCH);
        env.init();
    }
    
    virtual void TearDown() {
        env.uninit();
    }
    
protected:
    env_dmjson env;
};

TEST_F(frame_dmjson, dmjson) {
    // 1. 创建 JSON 对象
    json j;

    // 2. 基本数据类型写入
    j["name"] = "张三";
    j["age"] = 25;
    j["height"] = 175.5;
    j["is_student"] = true;
    j["address"] = nullptr;

    // 3. 数组操作
    j["hobbies"] = json::array({ "读书", "游泳", "编程" });
    j["scores"].push_back(95);
    j["scores"].push_back(87);
    j["scores"].push_back(92);

    // 4. 嵌套对象
    j["contact"]["phone"] = "13800138000";
    j["contact"]["email"] = "zhangsan@example.com";

    // 5. 使用初始化列表创建复杂 JSON
    json j2 = {
        {"students", {
            {
                {"id", 1001},
                {"name", "李四"},
                {"grades", {
                    {"math", 95},
                    {"english", 88},
                    {"chinese", 92}
                }}
            },
            {
                {"id", 1002},
                {"name", "王五"},
                {"grades", {
                    {"math", 87},
                    {"english", 94},
                    {"chinese", 89}
                }}
            }
        }},
        {"class_name", "高三一班"},
        {"teacher", "赵老师"}
    };

    // 6. JSON 序列化 - 转换为字符串
    std::string json_str = j.dump();
    std::string json_str_pretty = j.dump(4); // 格式化输出，缩进4个空格

    fmt::print("压缩格式: {}\n", json_str);
    fmt::print("格式化输出:\n{}\n", json_str_pretty);

    // 7. JSON 反序列化 - 从字符串解析
    std::string input_json = R"({
        "product": "笔记本电脑",
        "price": 5999.99,
        "specs": {
            "cpu": "Intel i7",
            "memory": "16GB",
            "storage": "512GB SSD"
        },
        "available": true,
        "tags": ["办公", "游戏", "学习"]
    })";

    try {
        json parsed_json = json::parse(input_json);

        // 8. 安全访问 JSON 数据
        if (parsed_json.contains("product")) {
            std::string product = parsed_json["product"];
            EXPECT_EQ(product, "笔记本电脑");
        }

        if (parsed_json.contains("price") && parsed_json["price"].is_number()) {
            double price = parsed_json["price"];
            EXPECT_DOUBLE_EQ(price, 5999.99);
        }

        // 9. 访问嵌套对象
        if (parsed_json.contains("specs") && parsed_json["specs"].is_object()) {
            std::string cpu = parsed_json["specs"]["cpu"];
            EXPECT_EQ(cpu, "Intel i7");
        }

        // 10. 遍历数组
        if (parsed_json.contains("tags") && parsed_json["tags"].is_array()) {
            for (const auto& tag : parsed_json["tags"]) {
                fmt::print("标签: {}\n", tag.get<std::string>()); // 假设 tag 是可以转换为 string 的
            }
            EXPECT_EQ(parsed_json["tags"].size(), 3);
        }

        // 11. 使用 at() 方法安全访问（会抛出异常）
        try {
            std::string cpu = parsed_json.at("specs").at("cpu");
            EXPECT_EQ(cpu, "Intel i7");
        }
        catch (const json::exception& e) {
            fmt::print(stderr, "JSON 访问错误: {}\n", e.what());
        }

        // 12. 使用 value() 方法提供默认值
        std::string brand = parsed_json.value("brand", "未知品牌");
        int warranty = parsed_json.value("warranty", 12); // 默认12个月

        EXPECT_EQ(brand, "未知品牌");
        EXPECT_EQ(warranty, 12);

    }
    catch (const json::parse_error& e) {
        fmt::print(stderr, "JSON 解析错误: {}\n", e.what());
        FAIL() << "JSON 解析失败";
    }

    // 13. 文件读写操作
    const std::string filename = "test_data.json";

    // 写入文件
    try {
        std::ofstream file(filename);
        if (file.is_open()) {
            file << j2.dump(4);
            file.close();
        }
    }
    catch (const std::exception& e) {
        fmt::print(stderr, "文件写入错误: {}\n", e.what());
    }

    // 从文件读取
    try {
        std::ifstream file(filename);
        if (file.is_open()) {
            json file_json;
            file >> file_json;
            file.close();

            // 验证读取的数据
            EXPECT_TRUE(file_json.contains("class_name"));
            EXPECT_EQ(file_json["class_name"], "高三一班");

            // 清理测试文件
            std::remove(filename.c_str());
        }
    }
    catch (const json::exception& e) {
        fmt::print(stderr, "文件读取错误: {}\n", e.what());
    }

    // 14. 类型检查
    json type_test = {
        {"string_val", "hello"},
        {"number_val", 42},
        {"float_val", 3.14},
        {"bool_val", true},
        {"null_val", nullptr},
        {"array_val", {1, 2, 3}},
        {"object_val", {{"key", "value"}}}
    };

    EXPECT_TRUE(type_test["string_val"].is_string());
    EXPECT_TRUE(type_test["number_val"].is_number_integer());
    EXPECT_TRUE(type_test["float_val"].is_number_float());
    EXPECT_TRUE(type_test["bool_val"].is_boolean());
    EXPECT_TRUE(type_test["null_val"].is_null());
    EXPECT_TRUE(type_test["array_val"].is_array());
    EXPECT_TRUE(type_test["object_val"].is_object());

    // 15. JSON Patch 操作示例
    json original = { {"name", "原始值"}, {"count", 10} };
    json patch = json::array({
        {{"op", "replace"}, {"path", "/name"}, {"value", "新值"}},
        {{"op", "add"}, {"path", "/description"}, {"value", "添加的描述"}}
        });

    // 注意: JSON Patch 需要额外的库支持，这里仅作示例

    // 16. 遍历 JSON 对象
    json obj = { {"a", 1}, {"b", 2}, {"c", 3} };
    for (auto& [key, value] : obj.items()) {
        // 假设 value 可以安全地转换为其字符串表示形式，或你希望其 JSON 表示
        // 如果 value 是复杂类型，直接传递给 fmt::print 可能需要 nlohmann/json 对 ostream 的支持
        // 或者显式转换为字符串: value.dump()
        // 对于基本类型，这通常可以正常工作
        fmt::print("键: {}, 值: {}\n", key, value.dump());
    }

    // 17. JSON 合并
    json base = { {"name", "base"}, {"version", 1} };
    json update = { {"version", 2}, {"author", "更新者"} };
    base.update(update);

    EXPECT_EQ(base["version"], 2);
    EXPECT_EQ(base["author"], "更新者");
    EXPECT_EQ(base["name"], "base");

    fmt::print("所有 JSON 操作测试完成!\n");
}