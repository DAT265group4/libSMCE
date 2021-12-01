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

TEST_CASE("t1", "[SD]") {
    REQUIRE(1 == 1);
}

TEST_CASE("SD remove file", "[SD File]") {
    smce::Toolchain tc{SMCE_PATH};
    REQUIRE(!tc.check_suitable_environment());
    smce::Sketch sk{SKETCHES_PATH "T3", {
            .fqbn = "arduino:avr:nano",
            .legacy_preproc_libs = { {"SD"} }
    }};
    const auto ec = tc.compile(sk);
    if (ec)
        std::cerr << ec.message() << '\n' << tc.build_log().second << std::endl;
    REQUIRE_FALSE(ec);

    smce::Board br{};
    smce::BoardConfig bc{
            /* .pins = */{0},
            /* .gpio_drivers = */{ smce::BoardConfig::GpioDrivers{0, smce::BoardConfig::GpioDrivers::DigitalDriver{true, true}, std::nullopt} },
            {},
            /* .sd_cards = */{ smce::BoardConfig::SecureDigitalStorage{0, STORAGE_PATH } },
            {}
    };
    REQUIRE(br.configure(std::move(bc)));
    REQUIRE(br.attach_sketch(sk));
    REQUIRE(br.start());
    auto d0 = br.view().pins[0].digital();
    test_pin_delayable(d0, true, 16384, 1ms); // wait for the pin to be set
    REQUIRE(br.stop());
    std::cerr << br.runtime_log().second << std::endl;

    REQUIRE_FALSE(std::filesystem::exists(STORAGE_PATH "bar/baz"));

}


