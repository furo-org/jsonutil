#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include <nlohmann/json.hpp>
#include "jsonutil.hh"
#include <array>
#include <vector>
struct Point{
  double x,y,z;
  int id;
  JSON_MEMBER(x,y,z,id);
};

struct Triangle{
  Point p1,p2,p3;
  std::string name;
  JSON_MEMBER(p1,p2,p3,name);
};

struct Points{
  std::vector<Point> pts;
  std::string name;
  JSON_MEMBER(pts,name);
};

TEST_CASE("SimpleStruct"){
  Point pt1{1.1, 2.2, 3.3, 4};
  SECTION("member mapping in nlohmann::json"){
    nlohmann::json j=pt1;
    auto itr=j.begin();
    CHECK(itr.key()=="id");
    ++itr;
    CHECK(itr.key()=="x");
    ++itr;
    CHECK(itr.key()=="y");
    ++itr;
    CHECK(itr.key()=="z");
  }
  SECTION("member mapping in yos::map_json"){
    yos::map_json j=pt1;
    auto itr=j.begin();
    CHECK(itr.key()=="id");
    ++itr;
    CHECK(itr.key()=="x");
    ++itr;
    CHECK(itr.key()=="y");
    ++itr;
    CHECK(itr.key()=="z");
  }
  SECTION("native and nlohmann::json roundtrip"){
    nlohmann::json j=pt1;
    Point pt2=j;
    CHECK(pt1.x==pt2.x);
    CHECK(pt1.y==pt2.y);
    CHECK(pt1.z==pt2.z);
    CHECK(pt1.id==pt2.id);
  }
  SECTION("native and yos::array_json roundtrip"){
    yos::array_json j=pt1;
    CHECK(j.is_array());
    Point pt2=j;
    CHECK(pt1.x==pt2.x);
    CHECK(pt1.y==pt2.y);
    CHECK(pt1.z==pt2.z);
    CHECK(pt1.id==pt2.id);
  }
  SECTION("native and yos::map_json roundtrip"){
    yos::map_json j=pt1;
    CHECK(!j.is_array());
    Point pt2=j;
    CHECK(pt1.x==pt2.x);
    CHECK(pt1.y==pt2.y);
    CHECK(pt1.z==pt2.z);
    CHECK(pt1.id==pt2.id);
  }
  SECTION("native to yos::map_json to nlohmann:json to native"){
    yos::map_json j=pt1;
    CHECK(!j.is_array());
    nlohmann::json nj=j;
    Point pt2=nj;
    CHECK(pt1.x==pt2.x);
    CHECK(pt1.y==pt2.y);
    CHECK(pt1.z==pt2.z);
    CHECK(pt1.id==pt2.id);
  }
  SECTION("native to yos::array_json to nlohmann:json to native"){
    yos::array_json j=pt1;
    CHECK(j.is_array());
    nlohmann::json nj=j;
    Point pt2=nj;
    CHECK(pt1.x==pt2.x);
    CHECK(pt1.y==pt2.y);
    CHECK(pt1.z==pt2.z);
    CHECK(pt1.id==pt2.id);
  }
}

TEST_CASE("Array of struct"){
  std::array<Point,3> pts={{
      {1.1,2.2,3.3,4},
      {10.1,20.2,30.3,5},
      {15.1,25.2,35.3,5}}};
  SECTION("native and nlohmann::json roundtrip"){
    nlohmann::json j=pts;
    CHECK(j.is_array());
    CHECK(j.size()==3);
    std::array<Point,3> pts2=j;
    CHECK(pts[0].x ==pts2[0].x);
    CHECK(pts[1].y ==pts2[1].y);
    CHECK(pts[2].z ==pts2[2].z);
    CHECK(pts[2].id==pts2[2].id);
  }
  SECTION("native and yos::array_json roundtrip"){
    yos::array_json j=pts;
    CHECK(j.is_array());
    CHECK(j.size()==3);
    CHECK(j[0].is_array());
    std::array<Point,3> pts2=j;
    CHECK(pts[0].x ==pts2[0].x);
    CHECK(pts[1].y ==pts2[1].y);
    CHECK(pts[2].z ==pts2[2].z);
    CHECK(pts[2].id==pts2[2].id);
  }
  SECTION("native and yos::map_json roundtrip"){
    yos::map_json j=pts;
    CHECK(j.is_array());
    CHECK(j.size()==3);
    CHECK(!j[0].is_array());
    CHECK(!j[1].is_array());
    CHECK(!j[2].is_array());
    std::array<Point,3> pts2=j;
    CHECK(pts[0].x ==pts2[0].x);
    CHECK(pts[1].y ==pts2[1].y);
    CHECK(pts[2].z ==pts2[2].z);
    CHECK(pts[2].id==pts2[2].id);
  }
  SECTION("native to yos::map_json to nlohmann:json to native"){
    yos::map_json j=pts;
    CHECK(j.is_array());
    CHECK(j.size()==3);
    CHECK(!j[0].is_array());
    CHECK(!j[1].is_array());
    CHECK(!j[2].is_array());
    nlohmann::json nj=j;
    std::array<Point,3> pts2=j;
    CHECK(pts[0].x ==pts2[0].x);
    CHECK(pts[1].y ==pts2[1].y);
    CHECK(pts[2].z ==pts2[2].z);
    CHECK(pts[2].id==pts2[2].id);
  }
  SECTION("native to yos::array_json to nlohmann:json to native"){
    yos::array_json j=pts;
    CHECK(j.is_array());
    CHECK(j.size()==3);
    CHECK(j[0].is_array());
    nlohmann::json nj=j;
    std::array<Point,3> pts2=j;
    CHECK(pts[0].x ==pts2[0].x);
    CHECK(pts[1].y ==pts2[1].y);
    CHECK(pts[2].z ==pts2[2].z);
    CHECK(pts[2].id==pts2[2].id);
  }
}
TEST_CASE("vector of struct"){
  std::vector<Point> pts={{
      {1.1,2.2,3.3,4},
      {10.1,20.2,30.3,5},
      {15.1,25.2,35.3,5}}};
  SECTION("native and nlohmann::json roundtrip"){
    nlohmann::json j=pts;
    CHECK(j.is_array());
    CHECK(j.size()==3);
    std::vector<Point> pts2=j;
    CHECK(pts2.size()==pts.size());
    CHECK(pts[0].x ==pts2[0].x);
    CHECK(pts[1].y ==pts2[1].y);
    CHECK(pts[2].z ==pts2[2].z);
    CHECK(pts[2].id==pts2[2].id);
  }
  SECTION("native and yos::array_json roundtrip"){
    yos::array_json j=pts;
    CHECK(j.is_array());
    CHECK(j.size()==3);
    CHECK(j[0].is_array());
    std::vector<Point> pts2=j;
    CHECK(pts2.size()==pts.size());
    CHECK(pts[0].x ==pts2[0].x);
    CHECK(pts[1].y ==pts2[1].y);
    CHECK(pts[2].z ==pts2[2].z);
    CHECK(pts[2].id==pts2[2].id);
  }
  SECTION("native and yos::map_json roundtrip"){
    yos::map_json j=pts;
    CHECK(j.is_array());
    CHECK(j.size()==3);
    CHECK(!j[0].is_array());
    CHECK(!j[1].is_array());
    CHECK(!j[2].is_array());
    std::vector<Point> pts2=j;
    CHECK(pts2.size()==pts.size());
    CHECK(pts[0].x ==pts2[0].x);
    CHECK(pts[1].y ==pts2[1].y);
    CHECK(pts[2].z ==pts2[2].z);
    CHECK(pts[2].id==pts2[2].id);
  }
  SECTION("native to yos::map_json to nlohmann:json to native"){
    yos::map_json j=pts;
    CHECK(j.is_array());
    CHECK(j.size()==3);
    CHECK(!j[0].is_array());
    CHECK(!j[1].is_array());
    CHECK(!j[2].is_array());
    nlohmann::json nj=j;
    std::vector<Point> pts2=j;
    CHECK(pts2.size()==pts.size());
    CHECK(pts[0].x ==pts2[0].x);
    CHECK(pts[1].y ==pts2[1].y);
    CHECK(pts[2].z ==pts2[2].z);
    CHECK(pts[2].id==pts2[2].id);
  }
  SECTION("native to yos::array_json to nlohmann:json to native"){
    yos::array_json j=pts;
    CHECK(j.is_array());
    CHECK(j.size()==3);
    CHECK(j[0].is_array());
    nlohmann::json nj=j;
    std::vector<Point> pts2=j;
    CHECK(pts2.size()==pts.size());
    CHECK(pts[0].x ==pts2[0].x);
    CHECK(pts[1].y ==pts2[1].y);
    CHECK(pts[2].z ==pts2[2].z);
    CHECK(pts[2].id==pts2[2].id);
  }
}

TEST_CASE("array of struct to vector of struct"){
  std::array<Point,3> pts={{
      {1.1,2.2,3.3,4},
      {10.1,20.2,30.3,5},
      {15.1,25.2,35.3,5}}};
  SECTION("native and nlohmann::json roundtrip"){
    nlohmann::json j=pts;
    CHECK(j.is_array());
    CHECK(j.size()==3);
    std::vector<Point> pts2=j;
    CHECK(pts2.size()==pts.size());
    CHECK(pts[0].x ==pts2[0].x);
    CHECK(pts[1].y ==pts2[1].y);
    CHECK(pts[2].z ==pts2[2].z);
    CHECK(pts[2].id==pts2[2].id);
  }
  SECTION("native and yos::array_json roundtrip"){
    yos::array_json j=pts;
    CHECK(j.is_array());
    CHECK(j.size()==3);
    CHECK(j[0].is_array());
    std::vector<Point> pts2=j;
    CHECK(pts2.size()==pts.size());
    CHECK(pts[0].x ==pts2[0].x);
    CHECK(pts[1].y ==pts2[1].y);
    CHECK(pts[2].z ==pts2[2].z);
    CHECK(pts[2].id==pts2[2].id);
  }
  SECTION("native and yos::map_json roundtrip"){
    yos::map_json j=pts;
    CHECK(j.is_array());
    CHECK(j.size()==3);
    CHECK(!j[0].is_array());
    CHECK(!j[1].is_array());
    CHECK(!j[2].is_array());
    std::vector<Point> pts2=j;
    CHECK(pts2.size()==pts.size());
    CHECK(pts[0].x ==pts2[0].x);
    CHECK(pts[1].y ==pts2[1].y);
    CHECK(pts[2].z ==pts2[2].z);
    CHECK(pts[2].id==pts2[2].id);
  }
  SECTION("native to yos::map_json to nlohmann:json to native"){
    yos::map_json j=pts;
    CHECK(j.is_array());
    CHECK(j.size()==3);
    CHECK(!j[0].is_array());
    CHECK(!j[1].is_array());
    CHECK(!j[2].is_array());
    nlohmann::json nj=j;
    std::vector<Point> pts2=j;
    CHECK(pts2.size()==pts.size());
    CHECK(pts[0].x ==pts2[0].x);
    CHECK(pts[1].y ==pts2[1].y);
    CHECK(pts[2].z ==pts2[2].z);
    CHECK(pts[2].id==pts2[2].id);
  }
  SECTION("native to yos::array_json to nlohmann:json to native"){
    yos::array_json j=pts;
    CHECK(j.is_array());
    CHECK(j.size()==3);
    CHECK(j[0].is_array());
    nlohmann::json nj=j;
    std::vector<Point> pts2=j;
    CHECK(pts2.size()==pts.size());
    CHECK(pts[0].x ==pts2[0].x);
    CHECK(pts[1].y ==pts2[1].y);
    CHECK(pts[2].z ==pts2[2].z);
    CHECK(pts[2].id==pts2[2].id);
  }
}

TEST_CASE("Nested structure"){
  Triangle tri={
      {0,0,0,0},{1.1,2.2,3.3,1},{-3.3,-4.4,-5.5,2},"three points"
      };
  SECTION("member mapping"){
    nlohmann::json j=tri;
    CHECK(j.is_object());

    auto itm=j.begin();
    CHECK(itm.key()=="name");
    itm++;
    CHECK(itm.key()=="p1");
    CHECK(itm.value().is_object());
    itm++;
    CHECK(itm.key()=="p2");
    CHECK(itm.value().is_object());
    itm++;
    CHECK(itm.key()=="p3");
    CHECK(itm.value().is_object());

    CHECK(j["name"]=="three points");
    CHECK(j["p1"]["x"]==0);
    CHECK(j["p2"]["y"]==2.2);
    CHECK(j["p3"]["z"]==-5.5);
  }
  SECTION("nlohmann::json roundtrip"){
    nlohmann::json j=tri;
    Triangle tri2=j;
    CHECK(tri.name==tri2.name);
    CHECK(tri.p1.x==tri2.p1.x);
    CHECK(tri.p1.y==tri2.p1.y);
    CHECK(tri.p1.z==tri2.p1.z);
    CHECK(tri.p2.x==tri2.p2.x);
    CHECK(tri.p2.y==tri2.p2.y);
    CHECK(tri.p2.z==tri2.p2.z);
    CHECK(tri.p3.x==tri2.p3.x);
    CHECK(tri.p3.y==tri2.p3.y);
    CHECK(tri.p3.z==tri2.p3.z);
  }
  SECTION("yos::map_json roundtrip"){
    yos::map_json j=tri;
    Triangle tri2=j;
    CHECK(tri.name==tri2.name);
    CHECK(tri.p1.x==tri2.p1.x);
    CHECK(tri.p1.y==tri2.p1.y);
    CHECK(tri.p1.z==tri2.p1.z);
    CHECK(tri.p2.x==tri2.p2.x);
    CHECK(tri.p2.y==tri2.p2.y);
    CHECK(tri.p2.z==tri2.p2.z);
    CHECK(tri.p3.x==tri2.p3.x);
    CHECK(tri.p3.y==tri2.p3.y);
    CHECK(tri.p3.z==tri2.p3.z);
  }
}

TEST_CASE("Structure with std::vector"){
  Points tri={
    {{0,0,0,0},{1.1,2.2,3.3,1},{-3.3,-4.4,-5.5,2}},"three points"
  };
  SECTION("member mapping"){
    nlohmann::json j=tri;
    CHECK(j.is_object());

    auto itm=j.begin();
    CHECK(itm.key()=="name");
    itm++;
    CHECK(itm.key()=="pts");
    CHECK(itm.value().is_array());

    CHECK(j["pts"].size()==3);

    CHECK(j["name"]=="three points");
    CHECK(j["pts"][0]["x"]==0);
    CHECK(j["pts"][1]["y"]==2.2);
    CHECK(j["pts"][2]["z"]==-5.5);
  }
  SECTION("nlohmann::json roundtrip"){
    nlohmann::json j=tri;
    Points tri2=j;
    CHECK(tri.name==tri2.name);
    CHECK(tri2.pts.size()==3);
    for(int i=0,ec=tri2.pts.size();i!=ec;++i){
      CHECK(tri.pts[i].x==tri2.pts[i].x);
      CHECK(tri.pts[i].y==tri2.pts[i].y);
      CHECK(tri.pts[i].z==tri2.pts[i].z);
    }
  }
  SECTION("yos::map_json roundtrip"){
    yos::map_json j=tri;
    Points tri2=j;
    CHECK(tri.name==tri2.name);
    CHECK(tri2.pts.size()==3);
    for(int i=0,ec=tri2.pts.size();i!=ec;++i){
      CHECK(tri.pts[i].x==tri2.pts[i].x);
      CHECK(tri.pts[i].y==tri2.pts[i].y);
      CHECK(tri.pts[i].z==tri2.pts[i].z);
    }
  }
}
