
100.times { | x |

    if( fork() == nil ) then
        print x , "\n"
    else
        exit
    end

}
