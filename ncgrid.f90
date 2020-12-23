program ncgrid

  use netcdf
  implicit none

  integer :: ncstat
  character(80) :: ncfile
  integer :: ncid

  character(*), parameter :: clat = 'XLAT'
  character(*), parameter :: clon = 'XLON'
  integer :: ilat, ilon

  real, allocatable :: lats(:), lons(:)
  integer :: latstat, lonstat

  integer :: latdimid
  integer :: ny
  character(len = nf90_max_name) :: name

  interface 
    subroutine check(ncstat) 
      integer, intent(in) :: ncstat
    end subroutine
  end interface

  write(*, *) 'Enter netCDF file: '
  read(*, '(a)') ncfile
  ncfile = adjustl(ncfile)

  call check(nf90_open(ncfile, nf90_nowrite, ncid))

  call check(nf90_inq_dimid(ncid, "south_north", latdimid))

  call check(nf90_inquire_dimension(ncid, 4, len = ny))

  !!!@#@ Staight up broken
  !call check(nf90_inq_varid(ncid, clat, ilat))
  !call check(nf90_inq_varid(ncid, clon, ilon))


  !call check(nf90_inq_dimid(ncid, 'west_east', dimid))
  !call check(nf90_inquire_dimension(ncid, dimid, clat, len))

  !allocate(lats(), stat = latstat)

  !call check(nf90_get_var(ncid, ilat, lats))

  !call check(nf90_close(ncid))

end program ncgrid

subroutine check(ncstat)
  integer, intent(in) :: ncstat
  if(ncstat /= nf90_noerr) then 
    !write(*, '(A)') nf90_strerror(ncstat)
    stop "Status Check-Error"
  end if
end subroutine