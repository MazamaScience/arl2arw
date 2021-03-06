program arl2arw
! -----------------------------------------------------------------------------
!
! Program to convert the packed ARL meteorological file format to WRF netCDF
! More information for ARL packed metorological format: 
! - https://ready.arl.noaa.gov/hysplitusersgudaye/S141.htm
! - https://www.ready.noaa.gov/archives.php
! - ftp://arlftp.arlhq.noaa.gov/pub/archives/utility/chk_data.f
!
! -----------------------------------------------------------------------------
    use netcdf
    implicit none
! -----------------------------------------------------------------------------
! Unpacking

    real, allocatable :: real_data(:, :) ! 2d unpacked data 
    character(1), allocatable :: char_data(:) ! 1d packed data

    character(80) :: arl_file ! prompt file name
    character(50) :: label ! header label 
    character(3072) :: header ! file header

    integer :: year, month ! year and month
    integer :: day, hour ! day and hour
    character(4) :: var_desc ! descriptor of variable being written
    integer :: ifc ! No clue 
    integer :: level ! level of z 
    integer :: k, n ! place holders
    integer :: nexp ! integer scaling exp of packaed data array
    real :: var1 ! real value of array at (1, 1)
    real :: prec ! real precision of packed data array

    character(4) :: model ! data model character
    integer :: grid_num ! grid number
    integer :: z_coord ! z coordinate
    real :: pole_lat, pole_lon, ref_lat, ref_lon, grid_size, orient, & 
            tan_lat, sync_xp, sync_yp, sync_lat, sync_lon, reserved
    integer :: nx, ny, nz, nxy ! number of x, y points, z levels
    integer :: recl ! record length
    
    integer :: alstat ! /= 0 if not enough memory 
    integer :: iostat ! > 0 if not enough memory 

! -----------------------------------------------------------------------------
! netCDF
    
    character(*), parameter :: ncpath = 'output.nc' 
    integer :: ncid 

    character(80) :: nc_file
    real, allocatable :: coord_data(:, :)

    integer :: lon_varid, lat_varid, rec_varid

    ! writing 4-d data lat-long grid with 
    character(*), parameter :: name_lat = 'latitude'
    character(*), parameter :: name_lon = 'longitude'
    character(*), parameter :: name_level = 'level'

    integer :: lvl_dimid, lon_dimid, lat_dimid, rec_dimid

    character(*), parameter :: units_lat = 'degrees_north'
    character(*), parameter :: units_lon = 'degrees_east'

    integer :: nx_id, ny_id, nz_id

    integer :: nlvls, nlats, nlons

    integer :: start(4)

    real, parameter :: start_lat = 25.0, start_lon = -119.0

    real :: lats(10), lons(10)
    integer :: lon, lat

    character(19) :: records 

! -----------------------------------------------------------------------------

    interface

        ! unpacked character data is resolved to real_data 
        subroutine unpack(char_data, real_data, nx, ny, nexp, var1)
            character(1), intent(in) :: char_data(:)
            real, intent(out) :: real_data(:, :)
            integer, intent(in) :: nx, ny, nexp
            real, intent(in) :: var1
        end subroutine

        ! wrapping netCDF calls with "call check()" checks the error status
        subroutine check(status)
            integer, intent(in) :: status
        end subroutine

    end interface

! -----------------------------------------------------------------------------
    
    ! get file name
    write(*,*) 'Enter ARL file name:'
    read(*, '(a)') arl_file
    arl_file = adjustl(arl_file)

    ! open the file and decode the 50-byte header plus the fixed portion
    open(10, file = arl_file, recl = 158, access = 'direct', form = 'unformatted')

    ! decode the "standard" portion of the index record
    read(10, rec = 1) label, header(1:108)
    read(label,'(5i2,4x,a4)') year, month, day, hour, ifc, var_desc
    write(*, '(a, 4i2)') 'YYMMDDHH: ', year, month, day, hour
    
    ! decode extended portion of header
    ! field descriptions: https://ready.arl.noaa.gov/hysplitusersguide/S141.htm
    read(header(1:108), '(a4, 1i3, 1i2,  12f7.0, 3i3)')         & 
         model, grid_num, z_coord,                              &
         pole_lat, pole_lon, ref_lat,                           &
         ref_lon, grid_size, orient,                            &
         tan_lat, sync_xp, sync_yp,                             &
         sync_lat, sync_lon, reserved,                          &
         nx, ny, nz  
    
    ! close the header read
    close(10)

    nxy = nx * ny ! record size
    recl = nxy + 50 ! record size + header length

    ! properly re-open the file with the record length
    open(10, file = arl_file, recl = recl, access = 'direct', form = 'unformatted')

    print *, 'Model: ', model, new_line('a'),                           & 
             'Grid Number: ', grid_num, new_line('a'),                  & 
             'Z-Coordinate: ', z_coord, new_line('a'),                  & 
             'Pole (lat,lon): ', pole_lat, pole_lon, new_line('a'),     & 
             'Reference (lat,lon): ', ref_lat, ref_lon, new_line('a'),  & 
             'Reference grid size: ', grid_size, new_line('a'),         &
             'Orientation: ', orient, new_line('a'),                    & 
             'Tangent Latitude: ', tan_lat, new_line('a'),              & 
             'Synch (x,y): ', sync_xp, sync_yp, new_line('a'),          & 
             'Synch (lat,lon): ', sync_lat, sync_lon, new_line('a'),    & 
             'Reserved: ', reserved, new_line('a'),                     &
             'Number of X points: ', nx, new_line('a'),                 & 
             'Number of Y points: ', ny, new_line('a'),                 & 
             'Number of Levels: ', nz, new_line('a')

    ! allocate the character and real arrarys of proper dimensions
    allocate(real_data(nx,ny), stat = alstat)
    allocate(char_data(nxy), stat = alstat)

    ! read the file and upack its content     
    n = 1 
    do
        ! read each header label
        read(10, rec = n, iostat = iostat) label, (char_data(k), k = 1, nxy) 
        if (iostat > 0) then ! >0 is error is thrown / end of records(?)
            exit 
        else
            read(label, '(6i2, 2x, a4, 1i4, 2f7.0)') & 
            year, month, day, hour, grid_num, level, & 
            var_desc, nexp, prec, var1
            ! unpack data from header 
            if(var_desc /= 'INDX')  then 
                call unpack(char_data, real_data, nx, ny, nexp, var1)   
            end if
            ! preview a bit first row of each data
            ! print *, level, var_desc, var1, real_data(2:3, 1), '...'
        end if
        n = n + 1
        ! TODO: implement netcdf writeout
    end do

    ! -------------------------------------------------------------------------

    write(*,*) 'Enter a NetCDF file name:'
    read(*, '(a)') nc_file
    nc_file = adjustl(nc_file)

    call check(nf90_open(nc_file, nf90_nowrite, ncid))

    ! Get the varids of the latitude and longitude coordinate variables.
    call check(nf90_inq_varid(ncid, 'XLAT', lat_varid))
    call check(nf90_inq_varid(ncid, 'XLON', lon_varid))

      ! Read the latitude and longitude data.
    call check( nf90_get_var(ncid, lat_varid, lats) )
    call check( nf90_get_var(ncid, lon_varid, lons) )

    ! print *, lats

    do rec = 1, 480 ! TODO dynamic allocate
        start(4) = rec
        call check(nf90_get_var(ncid, lat_varid, count = rec - 1))
        call check(nf90_get_var(ncid, lon_varid, count = rec - 1))

        i = 0
        do lvl = 1, nlvls
            do lat = 1, nlats
                do lon = 1, nlons
                    ! Write each level variable
                    i = i + 1
                end do
            end do
        end do
     ! next record
    end do
    ! -----------------------------------------------------------------------
    ! Header dump previews of ARL and WRF-netCDF

    !    wrf.arl
    !YYMMDDHH: 20112521
    !Model: AWRF
    !Grid Number:           21 
    !Z-Coordinate:            0 
    !Pole (lat,lon):    45.6899986      -121.000000     
    !Reference (lat,lon):    45.6899986      -121.000000     
    !Orientation:    0.00000000     
    !Tangent Latitude:    45.6899986     
    !Synch (x,y):    203.000000       141.500000     
    !Synch (lat,lon):    45.3499985      -118.930000     
    !Reserved:    0.00000000     
    !Number of X points:          405 
    !Number of Y points:          282 
    !Number of Levels:           38 

    !   wrf.nc 
    !$ ncdump -h wrfout_d3.2020112500.f21.0000
    !netcdf wrfout_d3.2020112500.f21 {
    !dimensions:
    !   Time = UNLIMITED ; // (1 currently)
    !   DateStrLen = 19 ;
    !   west_east = 405 ;
    !   south_north = 282 ;
    !   bottom_top = 37 ;
    !   bottom_top_stag = 38 ;
    !   soil_layers_stag = 4 ;
    !   west_east_stag = 406 ;
    !   south_north_stag = 283 ;
    !   seed_dim_stag = 2 ;
    ! -------------------------------------------------------------------------

    ! Create the nc file
    call check(nf90_create(ncpath, nf90_clobber, ncid))

    ! nc dimensions
    call check(nf90_def_dim(ncid, 'Time', nf90_unlimited, rec_dimid))
    call check(nf90_def_dim(ncid, 'west_east', nx, nx_id))
    call check(nf90_def_dim(ncid, 'south_north', ny, ny_id))
    call check(nf90_def_dim(ncid, 'bottom_top', nz - 1, nz_id))
    call check(nf90_def_dim(ncid, 'bottom_top_stag', nz, nz_id))

    ! nc variables
    call check(nf90_def_var(ncid, 'Time', nf90_char, rec_dimid, rec_varid))
    call check(nf90_def_var(ncid, 'XLAT', nf90_real, ny_id, lat_varid))
    call check(nf90_def_var(ncid, 'XLONG', nf90_real, nx_id, lon_varid))

    !call check(nf90_def_var(ncid, ''))

    ! nc attributes
    call check(nf90_put_att(ncid, lat_varid, 'description', 'LATITUDE, SOUTH IS NEGATIVE'))
    call check(nf90_put_att(ncid, lat_varid, 'units', 'degree_north'))
    call check(nf90_put_att(ncid, lon_varid, 'description', 'LONGITUDE, WEST IS NEGATIVE'))
    call check(nf90_put_att(ncid, lon_varid, 'units', 'degree_east'))

    ! end nc define 
    call check(nf90_enddef(ncid))

    ! Messing about
    ! do lat = 1, 10 
    !     lats(lat) = start_lat + (lat - 1) * 5.0
    ! end do
    ! do lon = 1, 10 
    !     lons(lon) = start_lon + (lon - 1) * 5.0
    ! end do

    ! !call check(nf90_out_var(ncid, rec_varid, records))
    ! call check(nf90_put_var(ncid, lon_varid, lons))
    ! call check(nf90_put_var(ncid, lat_varid, lats))

    ! close the nc file
    call check(nf90_close(ncid))
    
end program arl2arw

! -----------------------------------------------------------------------------
! Subroutines 
subroutine unpack(char_data, real_data, nx, ny, nexp, var1) 

    character(1), intent(in) :: char_data(:)    
    real, intent(out) :: real_data(:, :)
    integer, intent(in) :: nx, ny, nexp
    real, intent(in) :: var1
    real :: scale
    real :: vold
    integer :: indx
    integer :: i, j

    scale = 2.0 ** (7 - nexp)
    vold = var1 
    indx = 0

    do j = 1, ny
        do i = 1, nx 
            indx = indx + 1
            real_data(i,j) = (ichar(char_data(indx)) - 127.0) / scale + vold
            vold = real_data(i,j)
        end do
        vold = real_data(1,j)
    end do

end subroutine

subroutine check(status)

    integer, intent(in) :: status
    if(status /= nf90_noerr) then 
      stop "Check-Error"
    end if 

end subroutine

