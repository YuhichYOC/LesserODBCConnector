#include "stdafx.h"

#include "LesserODBCConnector.h"

void TestMethod01()
{
    LesserODBCConnector c;
    c.Prepare();
    c.Connect("");
    c.SQLNonQuery("");
}

int main(int argc, char * argv[])
{
    TestMethod01();
    return 0;
}