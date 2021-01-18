# arl2arw

**This program is still under development.**

arl2arw is program used for converting NOAA HYSPLIT compatible Air Resources Laboratory (_ARL_) Packed Data Format to WRF-like _Network Common Data Format_.

## Getting Started

The arl2arw program is tool designed to decompress ARL data format to WRF-like netCDF. Currently, the program does this using two inputs - an ARL compressed data file and a netCDF file with the desired time-coordinate grid. Running the program constructs a WRF-like netCDF file and copies the time-coordinate-space from the input netCDF. The input ARL packed data is then decompressed and stored to the output netCDF file.

### Compile

```bash
make compile
```

### Run 

```bash
./arl2arw YOUR_ARL_FILE YOUR_NC_FILE
```

### Notes

The repository contains a Makefile for simple compilation. arl2arw **requires** the C and the GCC compiler, as well as having the NetCDF C libraries to be installed.

## More Information

### NOAA Air Resource Laboratory Packed (ARL) Data Format

#### Data packing

ARL packed data files consist of numerous fixed-length records corresponding to each metoerological variable. The records are decribed at surface and upper-air levels and arranged in a time-series accordingly. Each record is compressed using a 50-byte ASCII header containing the date, time, variable, and data-packing constants.

The meterological data grid points is packed with a compression algorithm utilizing the packing constant to reduce the data to single-byte grid points where each byte is a representation of the difference in value between itself and previous point.

The algorithm is illustrated as follows: Assume that a grid of data is described with dimenisons `i,j`. The packed value is given by

`P_i,j = (R_i,j - R_i-1, j) * (2*exp(7-N))` [1]

Where `N` is the scaling component such that `N = ln(dR_max / ln(2))`, where `dR_max` is the maximum difference between any point on the grid and its previous. The "packed" values are represented as single-byte unsigned integers where values range from 0 to 254 such that 0 to 126 represent -127 to -1, 127 represents 0, and 128 to 254 represent +1 to +127. Furthermore each packed integer value is stored as charcter data represented by the corresponding integer. Note that the correspondance between chracters and their integer representation is not necessarily identical for different C compiler implementations[9] and is therefore recommended to use GCC. More information about ARL can be found in the resources[1].

#### Data Unpacking

Unpacking ARL data can be done by using the header to decode the index records using an unpacking algorithm described as the inverse packing algorithm: 

`U_i,j = ichar(P_i,j) - 127.0 / (2*exp(7-N) + P_1,1)` [5]

Where `ichar(P_i,j)` is the integer representation of the packed character data, `N` is the scaling component described above, and `P_1,1` is the first grid array element of the corresponding variable record. More infortmation about decoding ARL data files can be found in the resources [5].

#### ARL Data Description

##### Surface-Level Description

- `PRSS` Pressure at Surface (hPa)

- `SHGT` ??

- `T02M` Temperature at 2m (K)

- `U10M` U-component of wind at 10m (m/s)

- `V10M` V-component of wind at 10m (m/s)

- `PBLH` Boundary Layer Height (m)

- `TPP1` Total Precipitation 1hr (m)

- `SHTF` Sfc Sensible Heat Flux (W/m^2)

- `LHTF` Latent Heat Flux (W/m^2)

- `USTR` Friction Velocity (m/s)

- `RGHS` Surface Roughness (m)

- `DSWF` Downward Shortwave Flux (W/m^2)

##### Upper-Level Descriptions

- `PRES` Pressure (hPa)

- `UWND` U-component of wind (m/s)

- `VWND` V-component of wind (m/s)

- `WWND` Pressure vertical velocity (m/s)

- `TEMP` Temperature (K)

- `SPHU` Specific Humidity (kg/kg)

## Resources

- [1] <https://www.ready.noaa.gov/archives.php>

- [2] <https://www.arl.noaa.gov/hysplit/cmapf-mapping-routines/>

- [3] <https://www.arl.noaa.gov/hysplit/hysplit-frequently-asked-questions-faqs/faq-5/>

- [4] <https://www.arl.noaa.gov/hysplit/cmapf-mapping-routines/using-the-emapf-functions-fortran/>

- [5] <https://ready.arl.noaa.gov/hysplitusersguide/S141.htm>

- [6] <https://www.arl.noaa.gov/hysplit/cmapf-mapping-routines/cmapf-faq/>

- [7] <https://opensky.ucar.edu/islandora/object/opensky:2898>

- [8] <http://amps-backup.ucar.edu/information/configuration/wrf_grid_structure.html>