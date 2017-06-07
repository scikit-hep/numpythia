
cdef extern from "TDatabasePDG.h":
    cdef cppclass TDatabasePDG:
        pass
    cdef TDatabasePDG* TDatabasePDG_Instance "TDatabasePDG::Instance"()

cdef extern from "TObjArray.h":
    cdef cppclass TObjArray:
        int GetEntries()

cdef extern from "ExRootAnalysis/ExRootConfReader.h":
    cdef cppclass ExRootConfReader:
        ExRootConfReader()
        void ReadFile(const_char*)

cdef extern from "Delphes/modules/Delphes.h":
    cdef cppclass Delphes:
        Delphes(const_char*)
        void Clear()
        void InitTask()
        void ProcessTask()
        void SetConfReader(ExRootConfReader*)
        TObjArray* ExportArray(const_char*)
        TObjArray* ImportArray(const_char*)
