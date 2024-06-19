package main

import (
	"context"
	"encoding/json"
	"fmt"
	"net/http"
	"os/exec"
	"strconv"
	"time"

	"github.com/gin-gonic/gin"
	"go.mongodb.org/mongo-driver/mongo"
	"go.mongodb.org/mongo-driver/mongo/options"
)

var process *exec.Cmd

type pidReq struct {
	Pid int `json:"pid"`
}

func getDatos(context *gin.Context) {
	out, _ := exec.Command("cat", "/proc/p1_202113378").Output()
	context.IndentedJSON(http.StatusOK, string(out))
}

func insertarMongo(context *gin.Context) {
	// Lee el contenido del archivo
	out, err := exec.Command("cat", "/proc/p1_202113378").Output()
	var rawMessage json.RawMessage

	err = json.Unmarshal([]byte(out), &rawMessage)
	if err != nil {
		fmt.Println("Error al convertir el JsoN:", err)
	}
	fmt.Println("JSON sin procesar:", string(rawMessage))
}

func StartProcess(context *gin.Context) {
	cmd := exec.Command("sleep", "infinity")
	cmd.Start()
	process = cmd
	var res pidReq
	res.Pid = process.Process.Pid
	context.IndentedJSON(http.StatusOK, res)
}

func KillProcess(context *gin.Context) {
	var req pidReq
	context.BindJSON(&req)
	cmd := exec.Command("kill", "-9", strconv.Itoa(req.Pid))
	err := cmd.Run()
	if err != nil {
		context.IndentedJSON(http.StatusOK, err)
		return
	}
	context.IndentedJSON(http.StatusOK, "Proceso terminado")
}

var mongoClient *mongo.Client

func initMongo() {
	ClientOptions := options.Client().ApplyURI("mongodb://localhost:27017")
	client, err := mongo.NewClient(ClientOptions)
	if err != nil {
		fmt.Println("erro1")
		panic(err)
	}
	ctx, cancel := context.WithTimeout(context.Background(), 10*time.Second)
	defer cancel()
	err = client.Connect(ctx)
	if err != nil {
		fmt.Println("error2")
		panic(err)
	}
	mongoClient = client
	fmt.Println("todo ok")
}

func main() {
	initMongo()

	router := gin.Default()

	router.Use(func(c *gin.Context) {
		c.Writer.Header().Set("Access-Control-Allow-Origin", "*")
		c.Writer.Header().Set("Access-Control-Allow-Methods", "GET, POST, OPTIONS, PUT, DELETE")
		c.Writer.Header().Set("Access-Control-Allow-Headers", "Origin, Content-Type, Accept")
		c.Writer.Header().Set("Access-Control-Allow-Credentials", "true")

		if c.Request.Method == "OPTIONS" {
			c.AbortWithStatus(200)
			return
		}

		c.Next()
	})

	router.GET("/data", getDatos)
	router.GET("/iniciarP", StartProcess)
	router.POST("/terminarP", KillProcess)
	router.POST("/insertarMongo", insertarMongo)

	router.Run(":8080")
}
