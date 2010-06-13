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
        BreadCrumbTrail() : crumbs(0) {}
        size_t gathered() const { return crumbs; }
        bool empty() const { return trail.empty(); }
        coord_t size() const { return trail.size(); }
        void clear() { while(!empty()) trail.pop(); crumbs = 0; }

        void drop_crumb(coord_t x, coord_t y)
        {
            trail.push(Point(x, y));
        }

        Point gather_crumb()
        {
            Point p = next();
            trail.pop();
            ++crumbs;
            
            return p;
        }
    private:
        /* Member variables/attributes */
        size_t crumbs;
        std::queue<Point> trail;
        
        /* Member functions/methods */
        Point next() const { return trail.front(); }
};

} // oodles

#endif

