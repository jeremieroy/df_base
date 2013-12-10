#include <UnitTest++.h>
#include <ReportAssert.h>
#include <TestReporterStdout.h>

int main(int, char const *[])
{	
    int retVal = UnitTest::RunAllTests();	
	return retVal;
}
