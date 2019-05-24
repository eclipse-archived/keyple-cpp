#pragma once

#include <string.h>
#include <vector>

#if defined(WIN32) || defined(__MINGW32__) || defined(__MINGW64__)
#include <winscard.h>
#else
#include <PCSC/winscard.h>
#include <PCSC/wintypes.h>
#endif

/* Smartcard I/O */
#include "CardException.h"
#include "CardTerminal.h"

/* Common */
#include "Logger.h"
#include "LoggerFactory.h"

namespace org {
namespace eclipse {
namespace keyple {
namespace smartcardio {

using LoggerFactory = org::eclipse::keyple::common::LoggerFactory;
using Logger        = org::eclipse::keyple::common::Logger;

class CardTerminals {
protected:
    /**
	 *
	 */
    SCARDCONTEXT ctx;

private:
    /**
	 *
	 */
    std::vector<CardTerminal> terminals;

    /**
	 *
	 */
    const std::shared_ptr<Logger> logger = LoggerFactory::getLogger(typeid(CardTerminals));

public:
    /**
	 * Returns an unmodifiable list of all available terminals.
	 *
	 * @return an unmodifiable list of all available terminals.
	 *
	 * @throws CardException if the card operation failed
	 */
    std::vector<CardTerminal> list()
    {
        LONG ret;
        char *pszReaderList = nullptr;
        DWORD len;
        char *pszReader;

        logger->debug("listing current readers\n");
        ret = SCardEstablishContext(SCARD_SCOPE_USER, NULL, NULL, &this->ctx);
        if (ret != SCARD_S_SUCCESS)
        {
            logger->debug("error establishing context\n");
            return {};
        }

        terminals.empty();

        ret = SCardListReaders(ctx, NULL, NULL, &len);
        if (ret != SCARD_S_SUCCESS)
        {
            logger->debug("error listing readers\n");
            return {};
        }

        pszReaderList = (char *)calloc(len, sizeof(char));
        ret           = SCardListReaders(ctx, NULL, pszReaderList, &len);
        if (ret != SCARD_S_SUCCESS)
        {
            logger->debug("error listing readers (2)\n");
            return {};
        }


        pszReader = pszReaderList;
        while (*pszReader)
        {
            std::string s(pszReader);
            logger->debug("adding reader '%s' to list\n", s);
            terminals.push_back(CardTerminal(this->ctx, s));
            pszReader += strlen(pszReader) + 1;
        }

        free(pszReaderList);


        return terminals;
    }

    /**
	 * Constructor
	 *
	 * Constructs a new CardTerminals object.
	 *
	 * <p>This constructor is called by subclasses only. Application should
     * call {@linkplain TerminalFactory#terminals} to obtain a CardTerminals
	 * object.
     */
    CardTerminals()
    {
        logger->debug("constructor\n");
    }

    /**
     * Destructor
     */
    ~CardTerminals()
    {
        logger->debug("destructor\n");
    }
};

}
}
}
}
