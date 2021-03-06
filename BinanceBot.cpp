#include "BinanceBot.h"

void init();

auto main(int argc, char* argv[]) -> int
{
    cout << endl << "Welcome to the C++ Binance Bot. WARNING --> This is still an extremly early alpha build. Use at your own risk." << endl;
    
    init();
    return 0;
}

    
void init(){
    string input;
    cout << endl << "Choose bot mode: 1 = trading" << endl;
    cin >> input;
    (input == "1") ? bot.tradingBot() : bot.algoBot(); //TEMPORARY
}

void botData::setPair(){
    cout << "Select trading pair (ex: LTCBTC, TRXETH, BTCUSDT, etc): " << endl;
    cin >> bot.pair;
}

void botData::setPrice(double foundPrice){
    price = foundPrice;
}

void botData::setSellPercent(){
    cout << "Enter target gains percentage" << endl;
    cin >> sellPercent;
}

void botData::tradingBot(){
    string choice, amount;
    bot.setPair();
    bot.getPrice(pair);
    cout << "Selected " << pair << ". Last known price of " << pair << ": " << price;

    cout << "Buying or selling? (1 for buying, 0 for selling)" << endl;
    cin >> choice;
    (choice == "0") ? choice = "SELL" : choice = "BUY";
    
    cout << "Enter the price you would like to buy/sell at." << endl;
    cin >> price;
    
    cout << "Enter amount you would like to buy/sell." << endl;
    cin >> amount;
    
    string curlTest = "curl -H \"X-MBX-APIKEY: ";
  // string command = "\" -X POST 'https://api.binance.com/api/v3/order?symbol="+pair+"&side="+choice+"&type=LIMIT&timeInForce=GTC&quantity="+amount+"&price="+price+"&recvWindow=5000&timestamp=";
    getTime();
   //string message = "symbol="+pair+"&side="+choice+"&type=LIMIT&timeInForce=GTC&quantity="+amount+"&price="+price+"&recvWindow=5000&timestamp="+epochTime;
   // HMACsha256(message, bot.secretKey);
    //curlTest = curlTest+APIKEY+command+time+"&signature="+signature+"'";
    cout << endl << endl << curlTest << endl << endl;
   // system(curlTest.c_str());
    
    
}

void botData::algoBot(){
    algoCheck = false;
    algoBuy = true;
    bot.setPair();
    bot.setSellPercent();
    cout << "Gathering data on pair...This may take some time..." << endl;
    getPrice(bot.pair);
    cout << "PRICE: " << setprecision(8) << fixed << bot.price << endl;
     pastPrice = price;
    while(algoCheck == false)
         (algoBuy == true) ? checkBuy() : checkSell();
}

void botData::checkBuy(){
    bot.getHistoricalPrices();
    bot.calcRSI();
}

void botData::calcRSI()
{
    vector<long double> gain, loss, change, avgGain, avgLoss, RS, RSI;
    vector<long double> currentPeriod(close.end()-33, close.end());
    double sumGain = 0, sumLoss = 0;
    for(int i = 1; i < currentPeriod.size(); i++){
        change.push_back(currentPeriod[i] - currentPeriod[i-1]);
        cout << endl << currentPeriod[i] << endl;
    }
    cout << endl << "******* " << close.back() << endl;
    
    for(int i = 0; i < change.size(); i++){
        change[i] > 0 ? gain.push_back(change[i]) : gain.push_back(0);
        change[i] < 0 ? loss.push_back(abs(change[i])) : loss.push_back(0);
    }
    for(int i = 0; i < 14; i++){
        sumGain += gain[i];
        sumLoss += loss[i];
    }
    avgGain.push_back(sumGain/14);
    avgLoss.push_back(sumLoss/14);
    cout << endl << "gain: " << avgGain[0] << " loss: " << avgLoss[0] << endl;
    for(int i = 14, j = 1; i < gain.size(); i++){
        avgGain.push_back(((avgGain[j-1] * 13)+ gain[i])/14);
        avgLoss.push_back(((avgLoss[j-1] * 13)+ loss[i])/14);
        j++;
    }
    
    for(int i = 0; i < avgGain.size(); i++)
        RS.push_back(avgGain[i]/avgLoss[i]);
    
    for(int i = 0; i < RS.size(); i++)
        avgLoss[i] == 0 ? RSI.push_back(100) : RSI.push_back(100 - (100/(1+RS[i])));
    
    
    cout << endl << "RSI: " << RSI.back() << endl;
}

void botData::formatHistoricalPrices(json::value const &value){ //temp
    if(!value.is_null()){
        json::value historicalData = value;
        ofstream outputFile("test.txt"); //output to file
        outputFile << historicalData; //store JSON into file
        outputFile.close();
        ifstream inputFile("test.txt"); //input from file
        string historicalDataString="";
        string appendTemp;
        while(!inputFile.eof()){
        getline(inputFile,appendTemp); //store value from file into string
        historicalDataString.append(appendTemp);
        }
        boost::erase_first(historicalDataString, "["); //formatting for parsing
        historicalDataString.append("\"]"); //formatting for parsing
        int count = 0;
        cout << endl << endl << "------ " << historicalDataString << endl << endl;
        string tempHolder;
        for (string::iterator it = historicalDataString.begin(); it < historicalDataString.end(); it++){
            if(*it != '[' && *it != '"' && *it !=']'){
                if(*it != ',')
                    tempHolder.append(1,*it);
                else{
                    switch (count){
                        case 0:
                            openTime.push_back(stof(tempHolder.c_str()));
                            break;
                        case 1:
                            open.push_back(stof(tempHolder.c_str()));
                            break;
                        case 2:
                            high.push_back(stof(tempHolder.c_str()));
                            break;
                        case 3:
                            low.push_back(stof(tempHolder.c_str()));
                            break;
                        case 4:
                            close.push_back(stof(tempHolder.c_str()));
                            break;
                        case 5:
                            volume.push_back(stof(tempHolder.c_str()));
                            break;
                        case 6:
                            closeTime.push_back(stof(tempHolder.c_str()));
                            break;
                        case 7:
                            quoteAssetVolume.push_back(stof(tempHolder.c_str()));
                            break;
                        case 8:
                            numTrades.push_back(stof(tempHolder.c_str()));
                            break;
                        case 9:
                            takerBuyAssetVol.push_back(stof(tempHolder.c_str()));
                            break;
                        case 10:
                            takerBuyQuoteAssetVol.push_back(stof(tempHolder.c_str()));
                            break;
                        case 11:
                            Ignore.push_back(stof(tempHolder.c_str()));
                            break;
                        default:
                            break;
                    }//end of switch
                    (count == 11) ? count = 0 : count++;
                    tempHolder = "";
                }//end of else
            }//end of 2nd if
        } //end of for loop
    }//end of first if
}//End of function

void botData::getHistoricalPrices(){ //Get all price data since 1/1/2017 over an interval
    //https://api.binance.com/api/v1/klines?symbol=ETHBTC&interval=1h&startTime=1523059200
    http_client client(U(RESTfulHost));
    string interval, timestamp = "1483243199000";
    cout << endl << "Enter interval for data collection (Ex: 1h, 2h, 4h, 5m, etc)" << endl;
    cin >> interval;
    string full_request = "/api/v1/klines?symbol="+pair+"&interval="+interval;
    client.request(methods::GET, full_request).then([](http_response response)-> //Do a RESTful GET request on the full URL for the price
                                                    pplx::task<json::value>{ //Result will be in JSON
                                                        if(response.status_code() == status_codes::OK){ //If everything went okay, return the JSON (lambda function)
                                                            return response.extract_json();
                                                        }
                                                        return pplx::task_from_result(json::value());
                                                    })
    .then([](pplx::task<json::value> previousTask){
        try{
            json::value const & v = previousTask.get(); //Take the JSON gathered by the GET request and store it into CPP JSON object
            bot.formatHistoricalPrices(v);
        }
        catch (http_exception const & e){ //This is just here to output an error if one occurs.
            wcout << e.what() << endl;
        }
    }).wait(); //Wait for all to finish
}

void botData::checkSell(){
    getPrice(bot.pair);
    double difference;
    cout << endl << "PRICE: " << setprecision(8) << fixed << bot.price << endl;
    if(price > pastPrice){
        difference = ((price - pastPrice)/price)*100;
        cout << endl << "DIFF: " << difference << endl;
        
        if(difference >= sellPercent){
            cout << "We should sell." << endl;
        }
    }
}


void printPrice(json::value const &value){ //temp
    if(!value.is_null()){
        json::value test = value;
        string v = test["price"].as_string();
        double price = stof(v.c_str());
        bot.setPrice(price);
       // cout << test["price"] << endl << v << endl;
    }
    
}

void botData::getPrice(const string pair){
    http_client client(U(RESTfulHost)); //The base connection for the RESTful API
    string full_request = "/api/v3/ticker/price?symbol=" + pair; //The rest of the URL we are going to ping to the price
    client.request(methods::GET, full_request).then([](http_response response)-> //Do a RESTful GET request on the full URL for the price
                                                      pplx::task<json::value>{ //Result will be in JSON
                                                          if(response.status_code() == status_codes::OK){ //If everything went okay, return the JSON (lambda function)
                                                              return response.extract_json();
                                                          }
                                                          return pplx::task_from_result(json::value());
                                                      })
    .then([](pplx::task<json::value> previousTask){
        try{
            json::value const & v = previousTask.get(); //Take the JSON gathered by the GET request and store it into CPP JSON object
            printPrice(v); //pass that object to the print price
        }
        catch (http_exception const & e){ //This is just here to output an error if one occurs.
            wcout << e.what() << endl;
        }
    }).wait(); //Wait for all to finish
}

inline auto binary_to_hex_digit(unsigned a) -> char{
    return a + (a < 10 ? '0' : 'a' - 10);
}

auto binary_to_hex(unsigned char const* binary, unsigned binary_len) -> string {
    string r(binary_len * 2, '\0');
    for(unsigned i = 0; i < binary_len; ++i) {
        r[i * 2] = binary_to_hex_digit(binary[i] >> 4);
        r[i * 2 + 1] = binary_to_hex_digit(binary[i] & 15);
    }
    return r;
}

void botData::HMACsha256(string const& message, string const& key) {
    unsigned char result[EVP_MAX_MD_SIZE];
    unsigned result_len = 0;
    HMAC(EVP_sha256(), key.data(), key.size(), reinterpret_cast<unsigned char const*>(message.data()), message.size(), result, &result_len);
    signature = binary_to_hex(result, result_len);
}

void botData::getTime(){
    struct timeval tp;
    gettimeofday(&tp, NULL);
    long long mslong = (long long) tp.tv_sec * 1000L + tp.tv_usec / 1000;
    string time = to_string(mslong);
}
