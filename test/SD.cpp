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

TEST_CASE("SD add/remove file/dir", "[SD File]") {
    smce::Toolchain tc{SMCE_PATH};
    REQUIRE(!tc.check_suitable_environment());

    smce::Board br{};
    smce::BoardConfig bc{
        /* .pins = */ {0},
        /* .gpio_drivers = */
        {smce::BoardConfig::GpioDrivers{0, smce::BoardConfig::GpioDrivers::DigitalDriver{true, true}, std::nullopt}},
        {},
        /* .sd_cards = */
        {smce::BoardConfig::SecureDigitalStorage{0, STORAGE_PATH}},
        {}};

    REQUIRE(br.configure(std::move(bc)));

    SECTION("Add file") {
        smce::Sketch sk{SKETCHES_PATH "sd_fs", {.fqbn = "arduino:avr:nano", .legacy_preproc_libs = {{"SD"}}}};
        const auto ec = tc.compile(sk);
        if (ec)
            std::cerr << ec.message() << '\n' << tc.build_log().second << std::endl;
        REQUIRE_FALSE(ec);

        if (std::filesystem::exists(STORAGE_PATH))
            std::filesystem::remove_all(STORAGE_PATH);
        std::filesystem::create_directory(STORAGE_PATH);
        REQUIRE(br.attach_sketch(sk));
        REQUIRE(br.start());
        REQUIRE(br.stop());
        REQUIRE(std::filesystem::exists(STORAGE_PATH "foo"));
        REQUIRE(std::filesystem::is_directory(STORAGE_PATH "foo"));
        REQUIRE(std::filesystem::exists(STORAGE_PATH "bar"));
        REQUIRE(std::filesystem::is_directory(STORAGE_PATH "bar"));
        REQUIRE(std::filesystem::exists(STORAGE_PATH "dir_to_del"));
        REQUIRE(std::filesystem::is_directory(STORAGE_PATH "dir_to_del"));
        REQUIRE(std::filesystem::exists(STORAGE_PATH "bar/baz"));
        REQUIRE(std::filesystem::is_regular_file(STORAGE_PATH "bar/baz"));
        REQUIRE(std::filesystem::exists(STORAGE_PATH "bar/file_to_del"));
        REQUIRE(std::filesystem::is_regular_file(STORAGE_PATH "bar/file_to_del"));
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
        REQUIRE_FALSE(std::filesystem::exists(STORAGE_PATH "bar/file_to_del"));
        REQUIRE(std::filesystem::exists(STORAGE_PATH "bar"));

    }

    SECTION("Remove dir") {
        smce::Sketch sk{SKETCHES_PATH "sd_fs_rm_dir", {.fqbn = "arduino:avr:nano", .legacy_preproc_libs = {{"SD"}}}};
        const auto ec = tc.compile(sk);
        if (ec)
            std::cerr << ec.message() << '\n' << tc.build_log().second << std::endl;
        REQUIRE_FALSE(ec);

        REQUIRE(br.attach_sketch(sk));
        REQUIRE(br.start());
        REQUIRE(br.stop());
        REQUIRE_FALSE(std::filesystem::exists(STORAGE_PATH "dir_to_del"));
        REQUIRE(std::filesystem::exists(STORAGE_PATH "bar/baz"));

    }
}
