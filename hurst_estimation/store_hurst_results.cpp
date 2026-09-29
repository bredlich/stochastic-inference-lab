// Reads the Hurst validation and FTSE result CSVs already produced by
// hurst.cpp, and loads them into the shared PostgreSQL database.
// Date: 29/09/2026

#include <iostream>
#include <fstream>
#include <sstream>
#include "../shared_db/db.h"

int main() {
    try {
        pqxx::connection conn(getConnectionString());
        pqxx::work txn(conn);

        std::ifstream valFile("hurst_validation.csv");
        std::string line;
        std::getline(valFile, line); // skip header

        int rowCount = 0;
        while (std::getline(valFile, line)) {
            std::stringstream ss(line);
            std::string trueHStr, estHStr, errStr;
            std::getline(ss, trueHStr, ',');
            std::getline(ss, estHStr, ',');
            std::getline(ss, errStr, ',');

            txn.exec_params(
                "INSERT INTO hurst_validation_results (seed, true_h, estimated_h, absolute_error) "
                "VALUES ($1, $2, $3, $4)",
                42, std::stod(trueHStr), std::stod(estHStr), std::stod(errStr)
            );
            rowCount++;
        }

        std::ifstream ftseFile("hurst_ftse_result.csv");
        std::getline(ftseFile, line); // skip header
        std::getline(ftseFile, line);
        double ftseH = std::stod(line);

        std::string interpretation;
        if (ftseH > 0.55) interpretation = "Persistent, long-memory behaviour";
        else if (ftseH < 0.45) interpretation = "Anti-persistent, mean-reverting behaviour";
        else interpretation = "Consistent with a random walk";

        txn.exec_params(
            "INSERT INTO hurst_ftse_results (estimated_h, interpretation) VALUES ($1, $2)",
            ftseH, interpretation
        );

        txn.commit();
        std::cout << "Loaded " << rowCount << " validation rows and 1 FTSE result into Postgres." << std::endl;

    }
    catch (const std::exception& e) {
        std::cerr << "Database error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
