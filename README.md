# arl2arw

Convert ARL data format to WRF NetCDF

# Usage

arl2arw will display ARL packed data and unpack them for loading into a netCDF format.

To convert the data data we will need to compile and output the fortran program in a directory containing the ARL packaed data file. 

```
gfortran arl2arw.f90 -o arl2arw
./arl2arw
```

This should prompt the user for the name of the ARL data file within the current working directory.

Compiling this program requires the GNU Fortran compiler, gfortran.

## HYSPLIT

Air Resourcee Lab (ARL) trajectory model.

* [HYSPLIT Meteorological Data Conversion Utilities](https://www.ready.noaa.gov/HYSPLIT_data2arl.php)

## MeteoInfo

Jython/java project with utilities for data conversion.

* [MeteoInfo all GitHub repos](https://github.com/meteoinfo)
* [MeteoInfoLab script - convert ARL or concentration data to netCDF](https://hysplitbbs.arl.noaa.gov/viewtopic.php?t=1242)
* [convert2nc python wrapper](https://github.com/meteoinfo/MeteoInfo/blob/d580c01563027add2298d99b1b68642dfe3a516a/MeteoInfoLab/pylib/mipylib/dataset/midata.py#L599)
* [MeteoInfo java source code for org.meteoinfo.data.meteodata](https://github.com/meteoinfo/MeteoInfo/tree/master/MeteoInfoLib/src/main/java/org/meteoinfo/data/meteodata)

## AirFire

Robert Solomon initial stab at ingesting ARL data with `bulk_profiler_expanded_csv.F`:

* [https://github.com/pnwairfire/BulkARLProfiler](https://github.com/pnwairfire/BulkARLProfiler)
