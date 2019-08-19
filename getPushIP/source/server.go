package main

import (
	"encoding/json"
	"fmt"
	"io/ioutil"
	"log"
	"net/http"
	"os"
)

var oldIP string

type LocalIP struct{
	IP  string `json:"ip"`
};

func responIP(w http.ResponseWriter, req *http.Request){
	body, err := ioutil.ReadAll(req.Body)
	if err != nil{
		fmt.Println("ioutil.ReadAll Error", err)
		return
	}
	defer req.Body.Close()

	var p LocalIP
	if err = json.Unmarshal([]byte(body), &p); err != nil {
		fmt.Println("json.Unmarshal Error", err)
		fmt.Println(string(body))
		result := "{\"code\":400, \"msg\":\"ok\"}"
		w.Write([]byte(result))
		return
	}

	fmt.Printf("jacky's IP =%+v\n", p.IP);

	if(oldIP != p.IP){
		file,err := os.Create("ip.txt")
		if(err != nil){
			fmt.Println(err)
		}

		content := []byte(p.IP)

		err1 := ioutil.WriteFile("ip.txt", content, 0777)
		if(err1 != nil){
			fmt.Println(err1)
		}
		fmt.Println("write file successful")
		defer file.Close()

		oldIP = p.IP
	}
	fmt.Println("jacky's ip is as same as ip.txt on server")
}

func responWantIP(w http.ResponseWriter, req *http.Request){
	//read ip from local file
	//write data to client request
	fmt.Println("get New connection: ip:", req.RemoteAddr)

	newIP := LocalIP{
		IP:      oldIP,
	}

	data, err := json.Marshal(newIP)
	if err != nil {
		fmt.Printf("Marshal Error %v, ip:%v", err, newIP)
		return
	}
	w.Write([]byte(data))

}

func main() {

	file,err := os.Open("ip.txt")
	if(err != nil){
		fmt.Println(err)
	}

	body,err := ioutil.ReadAll(file)
	if(err != nil){
		fmt.Println(err)
	}
	fmt.Println("read ip.txt = " + string(body))
	oldIP = string(body)

	addr := "0.0.0.0:1111"
	http.HandleFunc("/ip", responIP)
	http.HandleFunc("/wantip", responWantIP)
	fmt.Println("Listen", addr)
	if err := http.ListenAndServe(addr, nil); err != nil {
		log.Fatal("ListenAndServe: ", err)
	}

}
