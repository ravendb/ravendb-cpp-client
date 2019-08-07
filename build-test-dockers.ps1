 Copy-Item '..\User\User.crt' 'User.crt'
 Copy-Item '..\User\User.key' 'User.key'
 docker build --tag "raven-cpp-test-linux" .\ -f dockerfile.linux
 Remove-Item 'User.key'
 Remove-Item 'User.crt'