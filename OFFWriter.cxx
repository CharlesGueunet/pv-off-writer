#include "OFFWriter.h"

#include "vtkDirectory.h"
#include "vtkIdList.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkUnstructuredGrid.h"
#include "vtksys/SystemTools.hxx"

#include <fstream>

vtkStandardNewMacro(OFFWriter);

// Public
// {{{

void OFFWriter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

// }}}
// Protected
// {{{

int OFFWriter::FillOutputPortInformation(int port, vtkInformation* info)
{
  switch (port)
    {
    case 0:
      info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkUnstructuredGrid");
      break;
    default:
      info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkDataObject");
      break;
    }

  return 1;
}

int OFFWriter::RequestData(vtkInformation*        request,
                           vtkInformationVector** inputVector,
                           vtkInformationVector*  outputVector)
{

   // Output = Input
  vtkUnstructuredGrid* inputData = vtkUnstructuredGrid::GetData(inputVector[0]);
  vtkInformation*      outputInfo = outputVector->GetInformationObject(0);
  vtkUnstructuredGrid* outputData = vtkUnstructuredGrid::SafeDownCast(
      outputInfo->Get(vtkDataObject::DATA_OBJECT()));
  // this plugin does not modify the input
  outputData->ShallowCopy(inputData);

  // Construt FileName
  if (FileName == "")
    {
      // Default name
      static unsigned occurenceNoName = 0;
      FileName = "SavedData_" + std::to_string(occurenceNoName++);
    }
  else
    {
      vtkSmartPointer<vtkDirectory> dir = vtkSmartPointer<vtkDirectory>::New();
      if (dir->FileIsDirectory(FileName.c_str()))
        {
          vtkErrorMacro("You should not pass a directory as output filename");
          return -1;
        }
    }

  std::string fileExtension =
      vtksys::SystemTools::GetFilenameLastExtension(FileName);

#ifdef WRITER_DEBUG
  if (fileExtension == "")
    {
      std::cout << "no extension, will add .off" << std::endl;
    }
  else
    {
      std::cout << "found extension : " << fileExtension << std::endl;
    }
#endif

  if (fileExtension == "")
    {
      fileExtension = ".off";
      FileName += fileExtension;
    }

#ifdef WRITER_DEBUG
  std::cout << "Data will be saved in : " << FileName << std::endl;
#endif

  // Get scalars
  vtkDataArray* scalars = inputData->GetPointData()->GetArray(ScalarField.c_str());

  // Write This
  WriteOFFFile(inputData, scalars);

  return 1;
}

void OFFWriter::WriteOFFFile(vtkUnstructuredGrid* inputData,
                             vtkDataArray*        scalars) const
{
  const int nbVerts = inputData->GetNumberOfPoints();
  const int nbCells = inputData->GetNumberOfCells();

  // Assume FileName is valid
  ofstream offFile;
  offFile.open(FileName);
  if(offFile.is_open())
    {
      offFile << "OFF" << endl;
      offFile << nbVerts << " " << nbCells << " " << 0 << endl;
      // Write vertices
      for (int i = 0; i < nbVerts; i++) {
         double coords[3];
         inputData->GetPoint(i, coords);
         double scalar = scalars->GetTuple1(i);
         offFile << coords[0] << " " << coords[1] << " " << coords[2] << " " << scalar << endl;
      }
      // Write cells
      for (int i = 0; i < nbCells; i++) {
          vtkSmartPointer<vtkIdList> points = vtkSmartPointer<vtkIdList>::New();
          inputData->GetCellPoints(i, points);
          const int nbCellVerts = points->GetNumberOfIds();
          offFile << nbCellVerts;
          for (int j = 0; j < nbCellVerts; j++) {
             offFile << " " << points->GetId(j);
          }
          offFile << endl;
      }
    }
  else
    {
       cerr << "[OFFWriter]: Unable to open file: " << FileName << endl;
    }
  offFile.close();
}

// }}}
