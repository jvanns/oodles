#ifndef OODLES_BREADCRUMBTRAIL_HPP
#define OODLES_BREADCRUMBTRAIL_HPP

// STL
#include <set>
#include <queue>

namespace oodles {

class BreadCrumbTrail
{
    public:
        /* Dependent typedefs */
        typedef int32_t coord_t;
        typedef std::pair<coord_t, coord_t> Point;

        /* Member functions/methods */
        coord_t size() const { return trail.size(); }
        bool empty() const { return trail.empty(); }
        void clear() { while(!empty()) trail.pop(); }

        void drop_crumb(coord_t x, coord_t y) { trail.push(Point(x, y)); }
        Point gather_crumb() { Point p = next(); trail.pop(); return p; }
    private:
        /* Member variables/attributes */
        std::queue<Point> trail;
        
        /* Member functions/methods */
        Point next() const { return trail.front(); }
};

} // oodles

#endif

