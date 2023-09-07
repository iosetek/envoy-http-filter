from flask import Flask, Response

app = Flask(__name__)

@app.route('/', methods=["GET", "POST"])
def hello_endpoint():
    return Response("Hello", 200)

if __name__ == '__main__':
    app.run(host="localhost", port=9000)
