/*
 * Copyright Elasticsearch B.V. and/or licensed to Elasticsearch B.V. under one
 * or more contributor license agreements. Licensed under the Elastic License;
 * you may not use this file except in compliance with the Elastic License.
 */

#ifndef INCLUDED_ml_maths_CDecompositionComponent_h
#define INCLUDED_ml_maths_CDecompositionComponent_h

#include <core/CMemory.h>
#include <core/CoreTypes.h>

#include <maths/CPRNG.h>
#include <maths/CSpline.h>
#include <maths/ImportExport.h>

#include <boost/optional.hpp>
#include <boost/ref.hpp>

#include <cstddef>
#include <vector>

namespace ml
{
namespace core
{
class CStatePersistInserter;
class CStateRestoreTraverser;
}
namespace maths
{

//! \brief Common functionality used by our decomposition component classes.
class MATHS_EXPORT CDecompositionComponent
{
    public:
        typedef maths_t::TDoubleDoublePr TDoubleDoublePr;
        typedef std::vector<double> TDoubleVec;
        typedef std::vector<CFloatStorage> TFloatVec;
        typedef CSpline<boost::reference_wrapper<const TFloatVec>,
                        boost::reference_wrapper<const TFloatVec>,
                        boost::reference_wrapper<const TDoubleVec> > TSplineCRef;
        typedef CSpline<boost::reference_wrapper<TFloatVec>,
                        boost::reference_wrapper<TFloatVec>,
                        boost::reference_wrapper<TDoubleVec> > TSplineRef;

    public:
        //! Persist state by passing information to \p inserter.
        void acceptPersistInserter(core::CStatePersistInserter &inserter) const;

        //! Create by traversing a state document.
        bool acceptRestoreTraverser(core::CStateRestoreTraverser &traverser);

    protected:
        //! \brief A low memory representation of the value and variance splines.
        class MATHS_EXPORT CPackedSplines
        {
            public:
                enum ESpline
                {
                    E_Value    = 0,
                    E_Variance = 1
                };

            public:
                typedef boost::array<CSplineTypes::EType, 2> TTypeArray;
                typedef boost::array<TFloatVec, 2> TFloatVecArray;
                typedef boost::array<TDoubleVec, 2> TDoubleVecArray;

            public:
                CPackedSplines(CSplineTypes::EType valueInterpolationType,
                               CSplineTypes::EType varianceInterpolationType);

                //! Create by traversing a state document.
                bool acceptRestoreTraverser(CSplineTypes::EBoundaryCondition boundary,
                                            core::CStateRestoreTraverser &traverser);

                //! Persist state by passing information to \p inserter.
                void acceptPersistInserter(core::CStatePersistInserter &inserter) const;

                //! An efficient swap of the contents of two packed splines.
                void swap(CPackedSplines &other);

                //! Check if the splines have been initialized.
                bool initialized(void) const;

                //! Clear the splines.
                void clear(void);

                //! Shift the spline values by \p shift.
                void shift(ESpline spline, double shift);

                //! Get a constant spline reference.
                TSplineCRef spline(ESpline spline) const;

                //! Get a writable spline reference.
                TSplineRef spline(ESpline spline);

                //! Get the splines' knot points.
                const TFloatVec &knots(void) const;

                //! Interpolate the value and variance functions on \p knots.
                void interpolate(const TDoubleVec &knots,
                                 const TDoubleVec &values,
                                 const TDoubleVec &variances,
                                 CSplineTypes::EBoundaryCondition boundary);

                //! Get a checksum for this object.
                uint64_t checksum(uint64_t seed) const;

                //! Debug the memory used by the splines.
                void debugMemoryUsage(core::CMemoryUsage::TMemoryUsagePtr mem) const;

                //! Get the memory used by these splines.
                std::size_t memoryUsage(void) const;

            private:
                //! The splines' types.
                TTypeArray m_Types;
                //! The splines' knots.
                TFloatVec m_Knots;
                //! The splines' values.
                TFloatVecArray m_Values;
                //! The splines' curvatures.
                TDoubleVecArray m_Curvatures;
        };

    protected:
        //! \param[in] maxSize The maximum number of component buckets.
        //! \param[in] boundaryCondition The boundary condition to use for the splines.
        //! \param[in] valueInterpolationType The style of interpolation to use for
        //! computing values.
        //! \param[in] varianceInterpolationType The style of interpolation to use for
        //! computing variances.
        CDecompositionComponent(std::size_t maxSize,
                                CSplineTypes::EBoundaryCondition boundaryCondition,
                                CSplineTypes::EType valueInterpolationType,
                                CSplineTypes::EType varianceInterpolationType);

        //! An efficient swap of the contents of two components.
        void swap(CDecompositionComponent &other);

        //! Check if the seasonal component has been estimated.
        bool initialized(void) const;

        //! Clear all data.
        void clear(void);

        //! Update the interpolation of the bucket values.
        //!
        //! \param[in] knots The spline knot points.
        //! \param[in] values The values at the spline knot points.
        //! \param[in] variances The variances at the spline knot points.
        void interpolate(const TDoubleVec &knots,
                         const TDoubleVec &values,
                         const TDoubleVec &variances);

        //! Shift the component's values by \p shift.
        void shiftLevel(double shift);

        //! Interpolate the function at \p time.
        //!
        //! \param[in] offset The offset for which to get the value.
        //! \param[in] n The bucket count containing \p offset.
        //! \param[in] confidence The symmetric confidence interval for the variance
        //! as a percentage.
        TDoubleDoublePr value(double offset, double n, double confidence) const;

        //! Get the mean value of the function.
        double meanValue(void) const;

        //! Get the variance of the residual about the function at \p time.
        //!
        //! \param[in] offset The offset for which to get the variance.
        //! \param[in] n The bucket count containing \p offset.
        //! \param[in] confidence The symmetric confidence interval for the
        //! variance as a percentage.
        TDoubleDoublePr variance(double offset, double n, double confidence) const;

        //! Get the mean variance of the function residuals.
        double meanVariance(void) const;

        //! Get the maximum ratio between a residual variance and the mean
        //! residual variance.
        double heteroscedasticity(void) const;

        //! Get the maximum size to use for the bucketing.
        std::size_t maxSize(void) const;

        //! Get the boundary condition to use when interpolating.
        CSplineTypes::EBoundaryCondition boundaryCondition(void) const;

        //! Get the value spline.
        TSplineCRef valueSpline(void) const;

        //! Get the variance spline.
        TSplineCRef varianceSpline(void) const;

        //! Get the underlying splines representation.
        const CPackedSplines &splines(void) const;

        //! Get a checksum for this object.
        uint64_t checksum(uint64_t seed) const;

    private:
        //! The minimum permitted size for the points sketch.
        static const std::size_t MIN_MAX_SIZE;

    private:
        //! The maximum number of buckets to use to cover the period.
        std::size_t m_MaxSize;

        //! The boundary condition to use for the splines.
        CSplineTypes::EBoundaryCondition m_BoundaryCondition;

        //! The spline we fit through the function points and the function point
        //! residual variances.
        CPackedSplines m_Splines;

        //! The mean value in the period.
        double m_MeanValue;

        //! The mean residual variance in the period.
        double m_MeanVariance;
};

}
}

#endif // INCLUDED_ml_maths_CDecompositionComponent_h