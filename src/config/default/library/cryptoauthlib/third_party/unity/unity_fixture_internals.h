/* Copyright (c) 2010 James Grenning and Contributed to Unity Project
 * ==========================================
 *  Unity Project - A Test Framework for C
 *  Copyright (c) 2007 Mike Karlesky, Mark VanderVoord, Greg Williams
 *  [Released under MIT License. Please refer to license.txt for details]
 * ========================================== */

#ifndef UNITY_FIXTURE_INTERNALS_H_
#define UNITY_FIXTURE_INTERNALS_H_

#ifdef __cplusplus
extern "C"
{
#endif

struct UNITY_FIXTURE_T
{
    int Verbose;
    int Silent;
    unsigned int RepeatCount;
    const char* NameFilter;
    int NameInvert;
    const char* GroupFilter;
    int GroupInvert;
};
extern struct UNITY_FIXTURE_T UnityFixture;

typedef void unityfunction(void);
void UnityTestRunner(unityfunction* setup,
                     unityfunction* testBody,
                     unityfunction* teardown,
                     const char* printableName,
                     const char* group,
                     const char* name,
                     const char* file, unsigned int line);

void UnityIgnoreTest(const char* printableName, const char* group, const char* name);
int UnityGetCommandLineOptions(int argc, const char* argv[]);
void UnityConcludeFixtureTest(void);

void UnityPointer_Set(void** pointer, void* newValue, UNITY_LINE_TYPE line);
void UnityPointer_UndoAllSets(void);
void UnityPointer_Init(void);
#ifndef UNITY_MAX_POINTERS
#define UNITY_MAX_POINTERS 5
#endif

#ifdef __cplusplus
}
#endif

#endif /* UNITY_FIXTURE_INTERNALS_H_ */
