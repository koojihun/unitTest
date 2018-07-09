// Copyright (c) 2011-2013 The Bitcoin Core developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#define BOOST_TEST_MODULE Bitcoin Test Suite

#include "main.h"
#include "random.h"
#include "txdb.h"
#include "ui_interface.h"
#include "util.h"
#ifdef ENABLE_WALLET
#include "db.h"
#include "wallet.h"
#endif

#include <boost/filesystem.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/thread.hpp>
#include <vector>

std::vector<CTxInfo> test_info_pool;

CClientUIInterface uiInterface;
CWallet* pWalletMain;

extern bool fPrintToConsole;
extern void noui_connect();

struct TestingSetup {
    CCoinsViewDB *pcoinsdbview;
    boost::filesystem::path pathTemp;
    boost::thread_group threadGroup;

    TestingSetup() {
		for (int cnt = 0; cnt < 100; cnt++) {
			CTxInfo tmp(cnt + 1, cnt + 1, cnt + 1);
			test_info_pool.push_back(tmp);
		}
        fPrintToDebugLog = false; // don't want to write to debug.log file
        SelectParams(CBaseChainParams::UNITTEST);
        noui_connect();
#ifdef ENABLE_WALLET
        bitdb.MakeMock();
#endif
		////////////////////////////////////////////////////////////////////////////////////////
        pathTemp = GetTempPath() / strprintf("test_bitcoin_%lu_%i", (unsigned long)GetTime(), (int)(GetRand(100000)));
		std::cout << "1";
        boost::filesystem::create_directories(pathTemp);
		std::cout << "1";
        mapArgs["-datadir"] = pathTemp.string();
		std::cout << "1";
        pcoinsdbview = new CCoinsViewDB(1 << 23, true);
		std::cout << "1";
        InitBlockIndex();
		std::cout << "1";
		////////////////////////////////////////////////////////////////////////////////////////
#ifdef ENABLE_WALLET
        bool fFirstRun;
        pWalletMain = new CWallet("wallet.dat");
        pWalletMain->LoadWallet(fFirstRun);
        RegisterValidationInterface(pWalletMain);
#endif
        nScriptCheckThreads = 3;
        for (int i=0; i < nScriptCheckThreads-1; i++)
            threadGroup.create_thread(&ThreadScriptCheck);
        RegisterNodeSignals(GetNodeSignals());
    }
    ~TestingSetup()
    {
        threadGroup.interrupt_all();
        threadGroup.join_all();
        UnregisterNodeSignals(GetNodeSignals());
#ifdef ENABLE_WALLET
        delete pWalletMain;
        pWalletMain = NULL;
#endif
        delete pcoinsdbview;
#ifdef ENABLE_WALLET
        bitdb.Flush(true);
#endif
        boost::filesystem::remove_all(pathTemp);
    }
};

BOOST_GLOBAL_FIXTURE(TestingSetup);

void Shutdown(void* parg)
{
  exit(0);
}

void StartShutdown()
{
  exit(0);
}

bool ShutdownRequested()
{
  return false;
}
