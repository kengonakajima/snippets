package main

import(
	"mysql"
	"fmt"
	"os"
	"strings"
)

func Escape(str string) (esc string) {
    // loop through string a character at a time
	ary := strings.Split( str, "", -1 )
	var prev string
	prev = ""
    for _,s := range ary {
        switch s {
            // Detect unescaped quotes
        case "'", "\"":
            if prev != "\\" {
                // Add escape character
                esc += "\\"
            }
        }
        esc += s
        prev = s
    }
    return
}


		

		
func main() {
	fmt.Printf("aho99\n");
//	dbh, err := mysql.Connect( "unix", "", "/var/lib/mysql/mysql.sock", "gotest", "gotest","hoge" )
	dbh, err := mysql.Connect( "unix", "", "/opt/local/var/run/mysql5/mysqld.sock", "root", "","test" )	
	if err != nil {
		fmt.Printf( "%s\n", err )
		os.Exit(1)
	}


	fmt.Printf("aho\n");
	dbh.Use("test")
	fmt.Printf("aho2\n");
	res := new(mysql.MySQLResponse)
	fmt.Printf("aho3\n");	
	res, err = dbh.Query( "SET NAMES utf8")
	fmt.Printf("aho4\n");	
	res, err = dbh.Query( "DROP TABLE IF EXISTS pl " )
	res, err = dbh.Query( "CREATE TABLE pl ( id int, name char(32) ) engine=InnoDB default charset=utf8")
	fmt.Printf("%s\n", res )

	for i := 0; i< 10; i++ {
		q := fmt.Sprintf( "INSERT INTO pl set id=%d, name='%s'", i, Escape("ほ\"げ" ))
		res, err = dbh.Query( q)
		fmt.Printf("query: %s\n", q )
		if err != nil {
			fmt.Printf("%s\n", err )
			os.Exit(1)
		}
	}

	res, err = dbh.Query( "select count(*) from pl" )
	fmt.Printf("%#v\n", res)
	for rowmap := res.FetchRowMap(); rowmap != nil; rowmap = res.FetchRowMap() {
		fmt.Printf("%#v\n", rowmap)
	}
	
	res, err = dbh.Query( "SELECT * FROM pl")
	fmt.Printf("%#v\n", res)

	fmt.Printf("RES: %#v\n",res )
	for rowmap := res.FetchRowMap(); rowmap != nil; rowmap = res.FetchRowMap() {
		fmt.Printf("%#v\n", rowmap)
	}

	res, err = dbh.Query("SHOW PROCESSLIST")
	for rowmap := res.FetchRowMap(); rowmap != nil; rowmap = res.FetchRowMap() {
		fmt.Printf("%#v\n", rowmap)
	}
	dbh.Quit()
}
