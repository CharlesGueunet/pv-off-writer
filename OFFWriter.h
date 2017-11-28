// .NAME OFFWriter.h - save an unstructured mesh on a .off file
// .SECTION Description
// OFF Writer is a writer for OFF. It take an unstructured grid as input.

#ifndef __vtkGenericWriter_h
#define __vtkGenericWriter_h

#include <string>

#include "vtkSmartPointer.h"
#include "vtkUnstructuredGridAlgorithm.h"

// writers
#include "vtkUnstructuredGrid.h"

class VTK_EXPORT OFFWriter : public vtkUnstructuredGridAlgorithm
{
public:
  static OFFWriter* New();
  vtkTypeMacro(OFFWriter, vtkUnstructuredGridAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  vtkSetMacro(FileName, std::string);
  vtkGetMacro(FileName, std::string);

  vtkSetMacro(ScalarField, std::string);
  vtkGetMacro(ScalarField, std::string);

protected:
  OFFWriter()  = default;
  ~OFFWriter() = default;

  // VTK Interface

  virtual int FillOutputPortInformation(int port, vtkInformation* info) override;

  int RequestData(vtkInformation*        request,
                  vtkInformationVector** inputVector,
                  vtkInformationVector*  outputVector) override;

  // methods
  void WriteOFFFile(vtkUnstructuredGrid* inputData,
                    vtkDataArray*        scalars) const;

  // fields

  std::string FileName;
  std::string ScalarField;

private:
  OFFWriter(const OFFWriter&); // Not implemented.
  void operator=(const OFFWriter&);   // Not implemented.
};

#endif
