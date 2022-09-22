#include <iostream>
#include "catch2/catch.hpp"
#include "geom/Parser.hh"
#include "geom/Logging.hh"

TEST_CASE("Parse define block", "[GDMLParser]") {
    // TODO: Figure out why default logger segfualts
    CreateLogger(false, 0, 1);
    std::string input = R"xml(
<?xml version="1.0"?>
<gdml xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="http://service-spi.web.cern.ch/service-spi/app/releases/GDML/schema/gdml.xsd">
  <define>
    <constant name="pi" value="3.14"/>
    <constant name="c" value="3e8"/>
    <position name="CScint_1inToppos" x="0" y="0" z="0" unit="cm"/>
    <position name="CScint_2inToppos" x="0" y="0" z="75" unit="cm"/>
    <position name="CScint_3inToppos" x="0" y="25" z="0" unit="cm"/>
    <position name="CScint_4inToppos" x="0" y="-25" z="0" unit="cm"/>
  </define>
  <materials>
    <element Z="6" formula="C" name="carbon">
      <atom value="12.0107"/>
    </element>
    <element Z="7" formula="N" name="nitrogen">
      <atom value="14.0671"/>
    </element>
    <element Z="8" formula="O" name="oxygen">
      <atom value="15.999"/>
    </element>
    <element Z="1" formula="H" name="hydrogen">
      <atom value="1.00794"/>
    </element>
    <element name="argon" formula="Ar" Z="18">
        <atom value="39.9480"/>
    </element>

    <material formula="" name="CScint">
      <D value="1.043"/>
      <composite n="0.922" ref="carbon"/>
      <composite n="0.076" ref="hydrogen"/>
      <composite n="0.0006" ref="nitrogen"/>
      <composite n="0.0007" ref="oxygen"/>
    </material>

    <material formula="" name="Air">
      <D value="0.001225"/>
      <fraction n="0.781154" ref="nitrogen"/>
      <fraction n="0.209476" ref="oxygen"/>
      <fraction n="0.00934" ref="argon"/>
    </material>

  </materials>
  <solids>
    <box name="Top" x="300" y="300" z="300" lunit="cm"/>
    <box name="CScint0x2" x="100" y="20" z="50" lunit="cm"/>
    <box name="CScint0x4" x="100" y="20" z="10" lunit="cm"/>
  </solids>
  <structure>
    <volume name="CScint0x1">
      <auxiliary auxtype="SensDet" auxvalue="CScintBlob"/>
      <materialref ref="CScint"/>
      <solidref ref="CScint0x2"/>
    </volume>
    <volume name="CScint0x3">
      <auxiliary auxtype="SensDet" auxvalue="CScintBlob"/>
      <materialref ref="CScint"/>
      <solidref ref="CScint0x4"/>
    </volume>
    <volume name="Top">
      <materialref ref="Air"/>
      <solidref ref="Top"/>
      <physvol name="CScint_1" copynumber="1">
        <volumeref ref="CScint0x1"/>
        <positionref ref="CScint_1inToppos"/>
      </physvol>
      <physvol name="CScint_2" copynumber="1">
        <volumeref ref="CScint0x3"/>
        <positionref ref="CScint_2inToppos"/>
      </physvol>
      <physvol name="CScint_2" copynumber="2">
        <volumeref ref="CScint0x3"/>
        <positionref ref="CScint_3inToppos"/>
      </physvol>
      <physvol name="CScint_2" copynumber="3">
        <volumeref ref="CScint0x3"/>
        <positionref ref="CScint_4inToppos"/>
      </physvol>
    </volume>
  </structure>
  <setup name="default" version="1.0">
    <world ref="Top"/>
  </setup>
</gdml>)xml";

    pugi::xml_document doc;
    auto result = doc.load_string(input.c_str());
    if(!result)
        throw std::runtime_error("GDMLParser: Invalid file");
    NuGeom::GDMLParser parser(doc);

    SECTION("Properly parses constant definitions") {
        CHECK_THAT(parser.GetConstant("pi"), Catch::WithinAbs(3.14, 1e-16));
        CHECK_THAT(parser.GetConstant("c"), Catch::WithinAbs(3e8, 1e-16));
        CHECK_THROWS_WITH(parser.GetConstant("invalid"),
                          Catch::Equals("GDMLParser: Undefined constant invalid"));
    }
}
