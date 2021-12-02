//
// Created by ASUS on 11/18/2021.
//
#include <iostream>
#include <catch2/catch_test_macros.hpp>
//#include "Ardrivo/SD.h"
#include "defs.hpp"

#include <chrono>
#include <fstream>
#include <iostream>
#include <catch2/catch_test_macros.hpp>
#include "SMCE/Board.hpp"
#include "SMCE/BoardConf.hpp"
#include "SMCE/BoardView.hpp"
#include "SMCE/Toolchain.hpp"
#include "defs.hpp"

using namespace std::literals;

TEST_CASE("SD add and remove file", "[SD File]") {
    smce::Toolchain tc{SMCE_PATH};
    REQUIRE(!tc.check_suitable_environment());

    smce::Board br{};
    // clang-format off
    smce::BoardConfig bc{
            /* .pins = */{0},
            /* .gpio_drivers = */
                         {smce::BoardConfig::GpioDrivers{0,
                                                         smce::BoardConfig::GpioDrivers::DigitalDriver{true, true},
                                                         std::nullopt}},
                         {},
            /* .sd_cards = */
                         {smce::BoardConfig::SecureDigitalStorage{0, STORAGE_PATH}},
                         {}
    };
    // clang-format on
    REQUIRE(br.configure(std::move(bc)));

    SECTION("Add file") {
        // clang-format off
        smce::Sketch sk{SKETCHES_PATH "sd_fs", {
                .fqbn = "arduino:avr:nano",
                .legacy_preproc_libs = {{"SD"}}
        }};
        // clang-format on
        const auto ec = tc.compile(sk);
        if (ec)
            std::cerr << ec.message() << '\n' << tc.build_log().second << std::endl;
        REQUIRE_FALSE(ec);

        if (std::filesystem::exists(STORAGE_PATH))
            std::filesystem::remove_all(STORAGE_PATH);
        std::filesystem::create_directory(STORAGE_PATH);
        REQUIRE(br.attach_sketch(sk));
        REQUIRE(br.start());
        auto d0 = br.view().pins[0].digital();
        test_pin_delayable(d0, true, 16384, 1ms); // wait for the pin to be set
        REQUIRE(br.stop());
        std::cerr << br.runtime_log().second << std::endl;

        REQUIRE(std::filesystem::exists(STORAGE_PATH "foo"));
        REQUIRE(std::filesystem::is_directory(STORAGE_PATH "foo"));
        REQUIRE(std::filesystem::exists(STORAGE_PATH "bar"));
        REQUIRE(std::filesystem::is_directory(STORAGE_PATH "bar"));
        REQUIRE(std::filesystem::exists(STORAGE_PATH "bar/baz"));
        REQUIRE(std::filesystem::is_regular_file(STORAGE_PATH "bar/baz"));
        std::ifstream baz{STORAGE_PATH "bar/baz"};
        REQUIRE(baz.is_open());
        std::string s;
        baz >> s;
        REQUIRE(s == "quxx");
    }
    SECTION("Remove file") {
        smce::Sketch sk{SKETCHES_PATH "sd_fs_rm_f", {.fqbn = "arduino:avr:nano", .legacy_preproc_libs = {{"SD"}}}};
        const auto ec = tc.compile(sk);
        if (ec)
            std::cerr << ec.message() << '\n' << tc.build_log().second << std::endl;
        REQUIRE_FALSE(ec);

        REQUIRE(br.attach_sketch(sk));
        REQUIRE(br.start());
        REQUIRE(br.stop());
        REQUIRE_FALSE(std::filesystem::exists(STORAGE_PATH "bar/baz"));
    }
}
