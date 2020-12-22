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

  interface 
    subroutine check(ncstat) 
      integer, intent(in) :: ncstat
    end subroutine
  end interface

  write(*, *) 'Enter netCDF file: '
  read(*, '(a)') ncfile
  ncfile = adjustl(ncfile)

  call check(nf90_open(ncfile, nf90_nowrite, ncid))

  call check(nf90_inq_varid(ncid, clat, ilat))
  call check(nf90_inq_varid(ncid, clon, ilon))

  call check(nf90_get_var(ncid, ilat, lats))

end program ncgrid

subroutine check(ncstat)
  integer, intent(in) :: ncstat
  if(status /= nf90_noerr) then 
    stop "Status Check-Error"
  end if
end subroutine