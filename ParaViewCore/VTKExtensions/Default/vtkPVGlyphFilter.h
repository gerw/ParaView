/*=========================================================================

  Program:   ParaView
  Module:    vtkPVGlyphFilter.h

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkPVGlyphFilter
 * @brief   extended API for vtkGlyph3D for better control
 * over glyph placement.
 *
 *
 * vtkPVGlyphFilter extends vtkGlyph3D for adding control over which points are
 * glyphed using \c GlyphMode. Three modes are now provided:
 * \li ALL_POINTS: all points in the input dataset are glyphed. This same as using
 * vtkGlyph3D directly.
 *
 * \li EVERY_NTH_POINT: every n-th point in the input dataset when iterated
 * through the input points sequentially is glyphed. For composite datasets,
 * the counter resets every on block. In parallel, independent counter is used
 * on each rank. Use \c Stride to control now may points to skip.
 *
 * \li SPATIALLY_UNIFORM_DISTRIBUTION: points close to a randomly sampled spatial
 * distribution of points are glyphed. \c Seed controls the seed point for the random
 * number generator (vtkMinimalStandardRandomSequence). \c MaximumNumberOfSamplePoints
 * can be used to limit the number of sample points used for random sampling. This
 * doesn't not equal the number of points actually glyphed, since that depends on
 * several factors. In parallel, this filter ensures that spatial bounds are collected
 * across all ranks for generating identical sample points.
*/

#ifndef vtkPVGlyphFilter_h
#define vtkPVGlyphFilter_h

#include "vtkPVVTKExtensionsDefaultModule.h" //needed for exports
#include "vtkPolyDataAlgorithm.h"

class vtkMultiProcessController;
class vtkTransform;

class VTKPVVTKEXTENSIONSDEFAULT_EXPORT vtkPVGlyphFilter : public vtkPolyDataAlgorithm
{
public:
  enum VectorScaleMode
  {
    SCALE_BY_MAGNITUDE,
    SCALE_BY_COMPONENTS
  };

  enum GlyphModeType
  {
    ALL_POINTS,
    EVERY_NTH_POINT,
    SPATIALLY_UNIFORM_DISTRIBUTION
  };

  vtkTypeMacro(vtkPVGlyphFilter, vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;
  static vtkPVGlyphFilter* New();

  //@{
  /**
   * Specify a source object at a specified table location. New style.
   * Source connection is stored in port 1. This method is equivalent
   * to SetInputConnection(1, id, outputPort).
   */
  void SetSourceConnection(int id, vtkAlgorithmOutput* algOutput);
  void SetSourceConnection(vtkAlgorithmOutput* algOutput)
  {
    this->SetSourceConnection(0, algOutput);
  }
  //@}

  //@{
  /**
   * Get/Set the vtkMultiProcessController to use for parallel processing.
   * By default, the vtkMultiProcessController::GetGlobalController() will be used.
   */
  void SetController(vtkMultiProcessController*);
  vtkGetObjectMacro(Controller, vtkMultiProcessController);
  //@}

  //@{
  /**
   * Get/set the range of the data. The first element defines the value that maps to glyph size
   * zero, and the second element defines the value that maps to the maximum glyph size. Data values
   * inside the range are mapped linearly to the range [0, MaximumGlyphSize], while data values
   * outside this range will be clamped to this range prior to mapping to the glyph size.
   */
  vtkSetVector2Macro(GlyphDataRange, double);
  vtkGetVector2Macro(GlyphDataRange, double);
  //@}

  //@{
  /**
   * Get/set the maximum glyph size. The upper data value specified in the GlyphDataRange
   * will map to this glyph size.
   */
  vtkSetMacro(MaximumGlyphSize, double);
  //@}

  //@{
  /**
   * Get/set whether to map scale array values to the glyph size range [0, MaximumGlyphSize].
   * If off, raw values from the scale array will be used if the scale array is set.
   */
  vtkSetMacro(RescaleGlyphs, bool);
  vtkGetMacro(RescaleGlyphs, bool);
  vtkBooleanMacro(RescaleGlyphs, bool);
  //@}

  //@{
  /**
   * Get/set the vector scaling mode. This mode determines how glyphs are scaled when the
   * scale array has more than one component.
   */
  //@}
  vtkSetClampMacro(VectorScaleMode, int, SCALE_BY_MAGNITUDE, SCALE_BY_COMPONENTS);
  vtkGetMacro(VectorScaleMode, int);

  //@{
  /**
   * When set, this is use to transform the source polydata before using it to
   * generate the glyph. This is useful if one wanted to reorient the source,
   * for example.
   */
  void SetSourceTransform(vtkTransform*);
  vtkGetObjectMacro(SourceTransform, vtkTransform);
  //@}

  /**
   * Overridden to include SourceTransform's MTime.
   */
  vtkMTimeType GetMTime() override;

  //@{
  /**
   * Set/get the desired precision for the output types. See the documentation
   * for the vtkAlgorithm::DesiredOutputPrecision enum for an explanation of
   * the available precision settings.
   */
  vtkSetMacro(OutputPointsPrecision, int);
  vtkGetMacro(OutputPointsPrecision, int);
  //@}

  //@{
  /**
   * Set/Get the mode at which glyphs will be generated.
   */
  vtkSetClampMacro(GlyphMode, int, ALL_POINTS, SPATIALLY_UNIFORM_DISTRIBUTION);
  vtkGetMacro(GlyphMode, int);
  //@}

  //@{
  /**
   * Set/Get the stride at which to glyph the dataset.
   * Note, only applicable with EVERY_NTH_POINT GlyphMode.
   */
  vtkSetClampMacro(Stride, int, 1, VTK_INT_MAX);
  vtkGetMacro(Stride, int);
  //@}

  //@{
  /**
   * Set/Get Seed used for generating a spatially uniform distribution.
   */
  vtkSetMacro(Seed, int);
  vtkGetMacro(Seed, int);
  //@}

  //@{
  /**
   * Set/Get maximum number of sample points to use to sample the space when
   * GlyphMode is set to SPATIALLY_UNIFORM_DISTRIBUTION.
   */
  vtkSetClampMacro(MaximumNumberOfSamplePoints, int, 1, VTK_INT_MAX);
  vtkGetMacro(MaximumNumberOfSamplePoints, int);
  //@}

  //@{
  /**
   * Overridden to create output data of appropriate type.
   */
  int ProcessRequest(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;

protected:
  vtkPVGlyphFilter();
  ~vtkPVGlyphFilter() override;
  //@}

  // Standard Pipeline methods
  int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;
  int RequestUpdateExtent(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;
  virtual int RequestDataObject(vtkInformation*, vtkInformationVector**, vtkInformationVector*);
  int FillInputPortInformation(int, vtkInformation*) override;
  int FillOutputPortInformation(int, vtkInformation*) override;

  /**
   * Returns 1 if point is to be glyphed, otherwise returns 0.
   */
  virtual int IsPointVisible(vtkDataSet* ds, vtkIdType ptId);

  /**
   * Returns true if input Scalars and Vectors are compatible, otherwise returns 0.
   */
  bool IsInputArrayToProcessValid(vtkDataSet* input);

  /**
   * Get the glyph source.
   */
  vtkPolyData* GetSource(int idx, vtkInformationVector* sourceInfo);

  /**
   * Returns true if input Scalars and Vectors are cell attributes, otherwise returns 0.
   */
  bool UseCellCenters(vtkDataSet* input);

  /**
   * Returns true if input scalars are used for glyphing.
   */
  bool NeedsScalars();

  /**
   * Returns true if input vectors are used for glyphing.
   */
  bool NeedsVectors();

  /**
   * Method called in RequestData() to do the actual data processing. This will
   * apply a Cell Centers before the Glyph. The \c input, filling up the \c output
   * based on the filter parameters.
   */
  virtual bool ExecuteWithCellCenters(
    vtkDataSet* input, vtkInformationVector* sourceVector, vtkPolyData* output);

  //@{
  /**
   * Method called in RequestData() to do the actual data processing. This will
   * glyph the \c input, filling up the \c output based on the filter
   * parameters.
   */
  virtual bool Execute(vtkDataSet* input, vtkInformationVector* sourceVector, vtkPolyData* output);
  virtual bool Execute(vtkDataSet* input, vtkInformationVector* sourceVector, vtkPolyData* output,
    vtkDataArray* inSScalars, vtkDataArray* inVectors);
  //@}

  int VectorScaleMode;
  vtkTransform* SourceTransform;
  double GlyphDataRange[2];
  double MaximumGlyphSize;
  bool RescaleGlyphs;
  int GlyphMode;
  int MaximumNumberOfSamplePoints;
  int Seed;
  int Stride;
  vtkMultiProcessController* Controller;
  int OutputPointsPrecision;

private:
  vtkPVGlyphFilter(const vtkPVGlyphFilter&) = delete;
  void operator=(const vtkPVGlyphFilter&) = delete;

  class vtkInternals;
  vtkInternals* Internals;
};

#endif
