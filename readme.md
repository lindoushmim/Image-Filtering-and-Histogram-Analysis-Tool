## Dependencies
`brew install opencv`
`brew install wxwidgets`

## Tester avec GUI (par default):

`mkdir build && cd build && cmake .. && make && ./MonProjet`


## Tester avec terminal :

`mkdir build && cd build && cmake .. -DUSE_GUI=OFF && make && ./MonProjet`
