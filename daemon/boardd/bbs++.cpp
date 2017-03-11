#include <string>
#include <vector>
#include "daemon/boardd/bbs++.hpp"
extern "C" {
#include "var.h"
#include "cmbbs.h"
#include "proto.h"
}

namespace paths {

std::string bfile(const char *brdname, const char *filename) {
  char buf[PATH_MAX];
  setbfile(buf, brdname, filename);
  return buf;
}

}  // namespace paths

namespace boards {

#define BOARD_HIDDEN(bptr) (bptr->brdattr & (BRD_HIDE | BRD_TOP) || \
    (bptr->level & ~(PERM_BASIC|PERM_CHAT|PERM_PAGE|PERM_POST|PERM_LOGINOK) && \
     !(bptr->brdattr & BRD_POSTMASK)))

bool IsPublic(const boardheader_t *bp) {
  return bp->brdname[0] && !BOARD_HIDDEN(bp);
}

boardheader_t *Get(bid_t bid) {
  return getbcache(bid);
}

bid_t Resolve(const std::string &name) {
  return getbnum(name.c_str());
}

std::vector<bid_t> Children(bid_t bid, const boardheader_t *bp) {
  std::vector<bid_t> bids;
  if (bp->brdattr & BRD_GROUPBOARD) {
    const int type = BRD_GROUP_LL_TYPE_CLASS;
    if (bp->firstchild[type] == 0) {
      resolve_board_group(bid, type);
    }
    for (bid = bp->firstchild[type]; bid > 0; bid = bp->next[type]) {
      bp = getbcache(bid);
      if (IsPublic(bp)) {
        bids.push_back(bid);
      }
    }
  }
  return bids;
}

}  // namespace boards